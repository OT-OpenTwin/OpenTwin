// @otlicense

#include "WindowsServiceManager.h"
#include "MongoDBSettingsParser.h"
#include "MongoDBUpgrader.h"
#include "MongoDBInstaller.h"
#include "MongoVersion.h"
#include "Logger.h"
#include "Varifier.h"
#include "OTCore/EncodingGuesser.h"
#include "OTCore/TextEncoding.h"
#include "OTCore/EncodingConverter_ISO88591ToUTF8.h"


#include <iostream>
#include <string>
#include <filesystem>
#include <mongocxx/instance.hpp>
#include <conio.h>
#include <any>
#include <boost/program_options.hpp>


//!brief:
//! Check if a mongodb service exists
//! if yes, get the configuration
//! Request the server regarding version
//! if version < 7 proceed
//! close service
//! Load configuration 
//! get connection properties
//! until version == 7
//!     start mongod server version x (async)
//!     create connection to server with driver
//!     upgrade fcv
//!     close server
//! Execute msi of mongodb 7
//! update config with setProperties ..
//! replace config of mongo 7
//! restart mongo 7

std::string g_serviceName = "MongoDB";
std::string Logger::m_loggingPath = ".\\MongoDBUpgrader.log";

//Exit codes :
//1: failed, the returned text holds the error message
//2: upgrade required, the returned text holds found version
//3: MongoDB found but no MongoDB data found
//4: Mongo version up - to - date
const int EXIT_CODE_FAILED = 1;
const int EXIT_CODE_UPGRADE_REQUIRED = 2;
const int EXIT_CODE_NO_UPGRADE = 3;
const int EXIT_CODE_MONGO_UP_TO_DATE = 4;


int requiresUpgrade(MongoVersion& _version)
{
    std::string serverName = g_serviceName;
    WindowsServiceManager mongoService(serverName);
    const std::string binPath = mongoService.getMongoDBServerBinPath();

    //Now we extract paths from the binPath
    MongoDBSettingsParser mongoSettingsParser(binPath);

    //Now we check the current MongoDB version
    _version.m_fullVersion = mongoSettingsParser.getCurrentMongoDBServiceVersion();

    //Now we analyse the current MongoDB version, if an update is necessary
    const std::string majorVersion = _version.m_fullVersion.substr(0, _version.m_fullVersion.find_first_of("."));
    _version.m_majorVersion = std::stoi(majorVersion);

    if (_version.m_majorVersion >= MongoDBUpgrader::getSupportedMaxVersion())
    {
        return EXIT_CODE_MONGO_UP_TO_DATE;
    }
    
    //The upgrade is really only necessary if the outdated MongoDB server already has data stored
    mongoSettingsParser.extractDataFromConfig();
    const MongoDBSettings& mongoSettings = mongoSettingsParser.getMongoDBSettings();
    //Now we check if there is already data stored by the current version. If so, we need to gradually upgrade the feature compatibility version.
    const std::string pathToDataStorage = mongoSettings.m_dataPath;
    if (std::filesystem::exists(pathToDataStorage) && !std::filesystem::is_empty(pathToDataStorage))
    {
        return EXIT_CODE_UPGRADE_REQUIRED;
    }
    else
    {
        return EXIT_CODE_NO_UPGRADE;
    }
}

void moveMongoCfg()
{
    std::string serverName = g_serviceName;
    WindowsServiceManager mongoService(serverName);
    const std::string binPath = mongoService.getMongoDBServerBinPath();
    mongoService.stopService();

    //Now we extract paths from the binPath
    MongoDBSettingsParser mongoSettingsParser(binPath);
    const std::string cfgPath = mongoSettingsParser.getTempMongoServerConfPath();

    if (!std::filesystem::exists(cfgPath))
    {
        throw std::exception(("MongoDb.cfg file not found: " + cfgPath).c_str());
    }
    else
    {
        const std::string destinationPath = mongoSettingsParser.getMongoDBSettings().m_configFilePath;
        Logger::INSTANCE().write("Removing automatically created cfg file: " + destinationPath + "\n");
        std::filesystem::remove(destinationPath);
        Logger::INSTANCE().write("Copying cfg with former settings to: " + destinationPath + "\n");
        std::filesystem::rename(cfgPath, destinationPath);
    }
}

void performUpgrade(const std::string& _adminPsw)
{
    std::string serverName = g_serviceName;
    WindowsServiceManager mongoService(serverName);
    const std::string binPath = mongoService.getMongoDBServerBinPath();

    //Now we extract paths from the binPath
    MongoDBSettingsParser mongoSettingsParser(binPath, _adminPsw);

    //Now we check the current MongoDB version
    MongoVersion version;
    version.m_fullVersion = mongoSettingsParser.getCurrentMongoDBServiceVersion();

    //Now we analyse the current MongoDB version, if an update is necessary
    const std::string majorVersion = version.m_fullVersion.substr(0, version.m_fullVersion.find_first_of("."));
    version.m_majorVersion = std::stoi(majorVersion);
    const std::string t = version.m_fullVersion.substr(version.m_fullVersion.find_first_of(".") + 1, version.m_fullVersion.size());
    version.m_minorVersion = t.substr(0, t.find_first_of("."));
    
    if (version.m_majorVersion >= MongoDBUpgrader::getSupportedMaxVersion())
    {
        throw std::exception("Upgrade not required. MongoDB version is already equal or above what this software suports.");
    }

    mongoSettingsParser.extractDataFromConfig();
    const MongoDBSettings& mongoSettings = mongoSettingsParser.getMongoDBSettings();
    //Now we check if there is already data stored by the current version. If so, we need to gradually upgrade the feature compatibility version.
    const std::string pathToDataStorage = mongoSettings.m_dataPath;
    if (std::filesystem::exists(pathToDataStorage) && !std::filesystem::is_empty(pathToDataStorage))
    {
        mongoService.stopService();
        mongocxx::instance instance{}; // This should be done only once.
        const std::string configPathForServerIteration = mongoSettingsParser.getTempMongoServerConfPath();
        mongoSettingsParser.createTempMongoServerConf(configPathForServerIteration);

        MongoDBUpgrader upgrader(mongoSettings, configPathForServerIteration);
        int foundFCV = upgrader.checkForFeatureCompatibilityVersion(version.m_majorVersion);
        if (foundFCV == 0)
        {
            throw std::exception("Failed to determine the current fcv.\n");
        }
        else if (foundFCV == MongoDBUpgrader::getSupportedMaxVersion())
        {
            throw std::exception(std::string("Upgrade not necessary, since MongoDB data feature compatibility version is already on version "+ std::to_string(foundFCV) + ".").c_str());
        }
        else
        {
            Logger::INSTANCE().write("Detected feature compatibility version: " + std::to_string(foundFCV) + "\n");
        }

        if (foundFCV == 4 && majorVersion == "4" && version.m_minorVersion == "2")
        {
            //std::cout << "\n";
            upgrader.performUpgrade4_2To4_4();
        }
        foundFCV++;


        for (int i = foundFCV; i <= MongoDBUpgrader::getSupportedMaxVersion(); i++)
        {
            upgrader.performUpgrade(i);
        }
    }
    else
    {
        throw std::exception("Upgrade not necessary, since MongoDB data directory is not existing or empty.");
    }
}

void close(int _exitCode)
{
    Logger::INSTANCE().write("Closing application.\n\n");
    exit(_exitCode);
}

void setServiceName(boost::program_options::variables_map& _arguments)
{
    if (_arguments.count("ServiceName") == 1)
    {
        const auto& value = _arguments["ServiceName"].value();
        g_serviceName = boost::any_cast<std::string>(value);
        Logger::INSTANCE().write("Using customised service name: " + g_serviceName+ "\n");
    }
    else
    {
        Logger::INSTANCE().write("Using default service name: " + g_serviceName + "\n");
    }
}


void varifyValidSettings(boost::program_options::variables_map& _arguments)
{    
    Logger::INSTANCE().write("Checking environment variable for MongoDB address.\n");
    Varifier varifier;
    varifier.ensureCorrectMongoEnvVar();    

    Logger::INSTANCE().write("Setting service permissions.\n");
    setServiceName(_arguments);
    WindowsServiceManager mongoService(g_serviceName);
    mongoService.stopService();

    const std::string binPath = mongoService.getMongoDBServerBinPath();
    //Now we extract paths from the binPath
    MongoDBSettingsParser mongoSettingsParser(binPath);
    mongoSettingsParser.extractDataFromConfig();

    varifier.setPermissions(mongoSettingsParser.getMongoDBSettings().m_dataPath);
    varifier.setPermissions(mongoSettingsParser.getMongoDBSettings().m_logPath);
    varifier.setPermissions(mongoSettingsParser.getMongoDBSettings().m_configFilePath);
}

int main(int argc, char* argv[])
{
    boost::program_options::options_description desc("Allowed options");

    desc.add_options()
        //Function 1: Check if upgrade is necessary
        ("Check", "Checks if an upgrade is necessary")
        //Function 2: Performs upgrade
        ("Upgrade", "Performce an upgrade of the database")
        //Function 3: Get max supported version
        ("MaxVersion", "Get the version that this software will upgrade MongoDB to.")
        //Function 4: Set mongodb.cfg
        ("SetMongoCfg", "Copy the updated mongodb.cfg into the new service directory.")
        //Function 5: Upgrade cfg
        //("UpgradeMongoCfg", "Upgrade the current mongodb cfg, so that it works with the new driver.") //ToDo
        //Function 5: Verify all MongoDB related settings
        ("VerifySetup", "Check if all MongoDB related settings are correct.") //ToDo
        //Parameter for upgrade
        ("AdminPsw", boost::program_options::value<std::string>(), "Admin Psw for the database")
        //Parameter for server name
        ("ServiceName", boost::program_options::value<std::string>(), "Name of the MongoDB service")
        ("LogPath", boost::program_options::value<std::string>(), "Path for the logfile.")
        ;

    boost::program_options::variables_map variableMap;
    try
    {
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), variableMap);
        if (variableMap.count("LogPath") == 1)
        {
            const auto& value = variableMap["LogPath"].value();
            std::string logPath = boost::any_cast<std::string>(value);
            Logger::Init(logPath + "\\MongoDBUpgrader.log");
        }
        else
        {
            Logger::Init(".\\MongoDBUpgrader.log");
        }
        Logger::INSTANCE().write("Starting MongoDBUpgrader.\n");
        if (variableMap.count("Check") == 1)
        {
            Logger::INSTANCE().write("Performing upgrade check\n");
            setServiceName(variableMap);
            MongoVersion version;
            int upgrade = requiresUpgrade(version);
            Logger::INSTANCE().write("Found version: " + version.m_fullVersion+ "\n");
            if (upgrade != EXIT_CODE_UPGRADE_REQUIRED)
            {
                Logger::INSTANCE().write("No upgrade required.\n");
            }
            
            std::cout << version.m_fullVersion;
            close(upgrade);
        }
        else if (variableMap.count("Upgrade") == 1)
        {
            Logger::INSTANCE().write("Performing upgrade.\n");
            setServiceName(variableMap);
            if (variableMap.count("AdminPsw") != 1)
            {
                std::cout << "AdminPsw must be set in order to upgrade";
                Logger::INSTANCE().write("Application is missing AdminPsw parameter\n");
                close(EXIT_CODE_FAILED);
            }
            else
            {
                auto& value = variableMap["AdminPsw"].value();
                std::string adminPSw = boost::any_cast<std::string>(value);
                ot::EncodingGuesser guesser;
                ot::TextEncoding::EncodingStandard standard = guesser(adminPSw.c_str(), adminPSw.size());
                if (standard == ot::TextEncoding::EncodingStandard::ANSI)
                {
                    Logger::INSTANCE().write("Admin password is given in ISO88591 encoding. Convert.");
                    ot::EncodingConverter_ISO88591ToUTF8 converter;
                    adminPSw = converter(adminPSw);
                }
                performUpgrade(adminPSw);
                close(ERROR_SUCCESS);
            }
        }
        else if (variableMap.count("SetMongoCfg") == 1)
        {
            Logger::INSTANCE().write("Setting mongodb.cfg of service.\n");
            setServiceName(variableMap);
            moveMongoCfg();
            close(ERROR_SUCCESS);
        }
        else if (variableMap.count("MaxVersion") == 1)
        {
            std::cout << MongoDBUpgrader::getSupportedMaxVersion();
            close(ERROR_SUCCESS);
        }
        else if (variableMap.count("VerifySetup") == 1)
        {
            Logger::INSTANCE().write("Verifying valid settings.\n");
            setServiceName(variableMap);
            varifyValidSettings(variableMap);
            close(ERROR_SUCCESS);
        }
        else
        {
            Logger::INSTANCE().write("Execution without parameter.\n");
            std::cout << "Missing parameter for the execution\n";
            close(EXIT_CODE_FAILED);
        }
    }
    catch (const std::exception& e)
    {
        std::cout <<"Error occured: "<< e.what();
        Logger::INSTANCE().write("Error occured: "+std::string(e.what())+"Closing application.\n\n");
        close(EXIT_CODE_FAILED);
    }
}
