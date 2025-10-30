// @otlicense
// File: Varifier.cpp
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

#include "Varifier.h"
#include "Logger.h"
#include "WindowsUtilityFuctions.h"
#include "SharedResources.h"

#include <string>
#include <iostream>
#include <boost/process/v1/system.hpp>
#include <stdlib.h>
void Varifier::ensureCorrectMongoEnvVar()
{
	CurrentPaths::INSTANCE().getMongoServerCollectionDirectory();
	Logger::INSTANCE().write("Search for MongoDB address env var: " + m_MongoAdressEnvVar + "\n");
		
	char* envVar;
	size_t envVarSize;
	errno_t err = _dupenv_s(&envVar, &envVarSize, m_MongoAdressEnvVar.c_str());
	if (err != NULL)
	{
		throw std::exception(("Failed to get Mongo env var " + m_MongoAdressEnvVar).c_str());
	}
	if (envVar != nullptr)
	{
		std::string mongoPath(envVar);
		Logger::INSTANCE().write("Env var found: "+ mongoPath + "\n");
		if (mongoPath.find("tls@") != std::string::npos)
		{
			mongoPath = mongoPath.substr(4);
			Logger::INSTANCE().write("Changing env var to: " + mongoPath + "\n");
			wuf::RegistryHandler handler("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment");
			handler.setStringVal(m_MongoAdressEnvVar, mongoPath);
		}
	}
}

void Varifier::setPermissions(const std::string& _path)
{
	std::string exeName = "C:\\Windows\\System32\\icacls.exe";
	std::string path = " \"" + _path + "\"";
	Logger::INSTANCE().write("Setting network permissions for: " + _path+ "\n");
	int returnVal =	boost::process::system(exeName + path +  " /grant NETWORKSERVICE:(OI)(CI)F /T");

	if (returnVal == ERROR_SUCCESS)
	{
		Logger::INSTANCE().write("Successfully set permissions for NETWORK SERVICE.\n");
	}
	else
	{
		Logger::INSTANCE().write("Failed to set permissions. Error: "+ wuf::getErrorMessage(returnVal) + "\n");
	}
}
