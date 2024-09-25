#include "MongoDBInstaller.h"
#include "SharedResources.h"
#include "WindowsServiceManager.h"
#include "MongoDBSettingsParser.h"
#include "FileWriter.h"

#include <boost/process.hpp>
#include <iostream>
#include "Logger.h"


MongoDBInstaller::MongoDBInstaller(const std::string& _serverName)
	:m_serverName(_serverName)
{}

bool MongoDBInstaller::installNewVersion(const std::string& _installPath)
{
    const std::string& executable = CurrentPaths::INSTANCE().getMongoNewVersionInstallationExe();
    boost::process::ipstream outputStream, errorStream;

    std::vector<std::string> args 
    {
        //"/qn",
        "/L*V", "C:\\Users\\JWagner\\Desktop\\mdbinstall.log",
        "/a", "\"" + executable + "\"",
        "SHOULD_INSTALL_COMPASS=\"0\"",
        "ADDLOCAL=\"ServerService,Client\"",
        "INSTALLLOCATION="
        "\""
        //"C:\\Program Files\\MongoDB\\Server\\7.0\\"
        "C:\\Users\\JWagner\\Desktop\\Temp"
        "\""
    };
    //C:\Users\JWagner\Documents\Workspace\Playground\MongoDB\MongoDBUpgrader\x64\Debug\MongoDB_Installer
    int exitCode = boost::process::system(boost::process::search_path("msiexec.exe"), boost::process::std_err > errorStream, args);
        // +_installPath,
        //"SHOULD_INSTALL_COMPASS=\"0\"",
        //"ADDLOCAL=\"ServerService,Client\"",
    return exitCode == ERROR_SUCCESS;
}

void MongoDBInstaller::replaceMongoCfg(const std::string& _configContent)
{
    
    Logger::INSTANCE().write("Searching for new MongoDB service.\n");
    WindowsServiceManager newMongoService(m_serverName);

    newMongoService.stopService();
    Logger::INSTANCE().write("Overwriting configuration file.\n");
    const std::string newBinPath = newMongoService.getMongoDBServerBinPath();
    MongoDBSettingsParser newMongoSettings(newBinPath);

    const std::string& configFilePath = newMongoSettings.getMongoDBSettings().m_configFilePath;
    
    
    FileWriter fileWriter(configFilePath);
    fileWriter.write(_configContent);
    Logger::INSTANCE().write("Restarting MongoDB service.\n");
    newMongoService.startService();
}
