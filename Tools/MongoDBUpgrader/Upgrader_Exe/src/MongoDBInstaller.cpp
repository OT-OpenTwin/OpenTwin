// @otlicense
// File: MongoDBInstaller.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
