// @otlicense

#include "MongoDBInstaller.h"
#include "SharedResources.h"
#include "WindowsServiceManager.h"
#include "MongoDBSettingsParser.h"
#include "FileWriter.h"
#include "Logger.h"

#include <boost/process.hpp>
#include <iostream>


MongoDBInstaller::MongoDBInstaller(const std::string& _serverName)
	:m_serverName(_serverName)
{}


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
