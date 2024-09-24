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

const std::string g_defaultServiceName = "MongoDB";

int main(int argc, wchar_t* argv[])
{

    //Function 1: Check if upgrade is necessary

    

    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("Check", "Checks if an upgrade is necessary")
        ("silent", "execution without asking the user for confirmation and installs MongoDB without user interaction.");

    boost::program_options::variables_map variableMap;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), variableMap);
    if (variableMap.count("adminPsw") == 1)
    {

    }
    bool silent = variableMap.count("silent") == 1;
    try
    {
        //First we extract information about the service named MongoDB
        std::cout << "Per default it is assumed that the MongoDB service is running by the name of \"" + g_defaultServiceName + "\" if that is correct, press enter, otherwise enter an alternative name.\n";
        std::string serverName = "";
        std::getline(std::cin, serverName);
        if (serverName == "")
        {
            serverName = g_defaultServiceName;
        }
        WindowsServiceManager mongoService(serverName);
        const std::string binPath = mongoService.getMongoDBServerBinPath();
//        std::cout << "Service runs with the binPath: " << binPath << "\n";
        //Now we extract paths from the binPath
        MongoDBSettingsParser mongoSettingsParser(binPath);

        //Now we check the current MongoDB version
        const std::string currentMongoVersion = mongoSettingsParser.getCurrentMongoDBServiceVersion();
        std::cout << "Current MongoDB server has the version " << currentMongoVersion << "\n";

        //Now we analyse the current MongoDB version, if an update is necessary
        const std::string majorVersion = currentMongoVersion.substr(0, currentMongoVersion.find_first_of("."));
        const std::string t = currentMongoVersion.substr(currentMongoVersion.find_first_of(".") + 1, currentMongoVersion.size());
        const std::string minorVersion = t.substr(0, t.find_first_of("."));


        
        int majorVersionInt = std::stoi(majorVersion);
        if (majorVersionInt >= MongoDBUpgrader::getSupportedMaxVersion())
        {
            std::cout << "No update required. Closing application.";
            _getch();
            exit(ERROR_SUCCESS);
        }

        mongoSettingsParser.extractDataFromConfig();
        const MongoDBSettings& mongoSettings = mongoSettingsParser.getMongoDBSettings();
        //Now we check if there is already data stored by the current version. If so, we need to gradually upgrade the feature compatibility version.
        const std::string pathToDataStorage = mongoSettings.m_dataPath;

        if (std::filesystem::exists(pathToDataStorage) && !std::filesystem::is_empty(pathToDataStorage))
        {
            std::cout << "Data folder contains old projects, it is necessary to upgrade the existing data. Please enter yes/y to proceed or no/n to cancel.\n";
            std::string input("");
            bool stayInLoop = true;
            while (stayInLoop)
            {
                std::getline(std::cin, input);
                if (input == "yes" || input == "y")
                {
                    stayInLoop = false;
                }
                else if (input == "no" || input == "n")
                {
                    return 0;
                }
            }
            std::cout << "Performing upgrade up to version " << MongoDBUpgrader::getSupportedMaxVersion() << "\n";
            std::cout << "\n!WARNING! Once you have upgraded to 7.0, you will not be able to downgrade FCV and binary version without support assistance.\n\n";
            std::cout << "Stopping running MongoDB service \n";
            mongoService.stopService();
            mongoSettingsParser.requestAdminPassword();
            mongocxx::instance instance{}; // This should be done only once.
            const std::string configPathForServerIteration = mongoSettingsParser.getTempMongoServerConfPath();
            

            MongoDBUpgrader upgrader(mongoSettings, configPathForServerIteration);
            
            if (majorVersion == "4" && minorVersion == "2")
            {
                std::cout << "\n";
                upgrader.performUpgrade4_2To4_4();
            }
            majorVersionInt++;

            for (int i = majorVersionInt; i <= MongoDBUpgrader::getSupportedMaxVersion(); i++)
            {
                std::cout << "\n";
                upgrader.performUpgrade(i);
            }
            
            //Now we (try) remove the temporary cfg file again
            try
            {
                bool success = std::filesystem::remove(configPathForServerIteration);
                if (!success)
                {
                    std::cout << "Failed to remove the temporary cfg file in installation dir: " + configPathForServerIteration + "\n";
                }
            }
            catch (std::filesystem::filesystem_error& error)
            {
                std::cout << "Failed to remove the temporary cfg file in installation dir: " + configPathForServerIteration + " because of: " + error.what() + "\n";
            }
        }
        
        //Now we install the new MongoDB version
        std::cout << "Starting installation of MongoDB " << MongoDBUpgrader::getSupportedMaxVersion() << ".\n";
        MongoDBInstaller installer(g_defaultServiceName);
        std::string mongoRootPath = mongoSettings.m_executablePath.substr(0, mongoSettings.m_executablePath.find_last_of("\\") - 1);
        mongoRootPath = mongoRootPath.substr(0, mongoRootPath.find_last_of("\\") - 1);
        const std::string installPath = mongoRootPath + "\\7.0";
        if(std::filesystem::exists(installPath) && !std::filesystem::is_empty(installPath))
        {
            std::cout << "An installation of MongoDB v. 7.0 already exists at the default location: " << installPath << " proceed manually with the installation.\n";
        }
        else
        {
            bool success = installer.installNewVersion(installPath);
        
            if (success)
            {
                std::cout << "Upgrade of MongoDB finished. Press key to close.\n";
                std::cin;
            }
            else
            {
                std::cout << "Installation failed. Closing application. Press key to close.";
                std::cin;
            }
            //Replacing the new mongod.cfg with an adjusted version of the old mongod.cfg
            installer.replaceMongoCfg(mongoSettingsParser.getUpdatedConfig());
        }
    }
    catch (std::exception& e)
    {
        std::cout <<"Application crashed with the following reason: " << e.what() << "\n Press key to close.";
        _getch();
    } 
}
