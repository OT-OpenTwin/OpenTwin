#include <iostream>
#include <string>
#include <filesystem>
#include "WindowsServiceManager.h"
#include "MongoDBSettingsParser.h"
#include "MongoDBUpgrader.h"
#include "mongocxx/instance.hpp"
#include "MongoDBInstaller.h"
#include <conio.h>
#include "boost/program_options.hpp"
#include "MongoVersion.h"
#include <any>
#include "Logger.h"
#include "Varifier.h"

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
    mongoService.startService();

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
        std::filesystem::remove(destinationPath);
        std::filesystem::rename(cfgPath, destinationPath);
    }
    mongoService.startService();
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

        if (majorVersion == "4" && version.m_minorVersion == "2")
        {
            std::cout << "\n";
            upgrader.performUpgrade4_2To4_4();
        }
        version.m_majorVersion++;

        for (int i = version.m_majorVersion; i <= MongoDBUpgrader::getSupportedMaxVersion(); i++)
        {
            std::cout << "\n";
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
    Varifier varifier;
    varifier.ensureCorrectMongoEnvVar();

}

int main(int argc, char* argv[])
{
    Logger::Init(".\\MongoDBUpgrader.log");
    
    Logger::INSTANCE().write("Starting MongoDBUpgrader.\n");

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
        ("UpgradeMongoCfg", "Upgrade the current mongodb cfg, so that it works with the new driver.") //ToDo
        //Function 5: Verify all MongoDB related settings
        ("VerifySetup", "Check if all MongoDB related settings are correct.") //ToDo
        //Parameter for upgrade
        ("AdminPsw", boost::program_options::value<std::string>(), "Admin Psw for the database")
        //Parameter for server name
        ("ServiceName", boost::program_options::value<std::string>(), "Name of the MongoDB service")
        ;

    boost::program_options::variables_map variableMap;
    try
    {
        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), variableMap);
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
                const auto& value = variableMap["AdminPsw"].value();
                const std::string adminPSw = boost::any_cast<std::string>(value);
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
            Logger::INSTANCE().write("Setting mongodb.cfg of service.\n");
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
