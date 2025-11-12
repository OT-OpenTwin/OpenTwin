// @otlicense
// File: FDTDLauncher.h
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

#pragma once

// TINYXML2
#include <tinyxml2.h>

// STD
#include <string>
#include <list>
#include <cstdint>
#include <map>

// OPENTWIN
#include "OTCore/CoreTypes.h"
#include "EntityInformation.h"
#include "EntityProperties.h"

namespace ot {
	namespace components {
		class ModelComponent;
	}
}

class Application;

class EntityBase;

class FDTDLauncher
{
public:
	FDTDLauncher(Application *app);
	virtual ~FDTDLauncher();

	std::string startSolver(std::string& logFileText, const std::string &dataBaseURL, const std::string &uiUrl, const std::string &projectName, EntityBase* solverEntity, int serviceID, int sessionCount, ot::components::ModelComponent *modelComponent);

private:
	std::string extractMesh(EntityBase* solverEntity, const std::string& tempDirPath, ot::UID & meshDataID, std::string& currentMeshDataName);
	void readGroupsFromMesh(const std::string& meshFileName, std::map<std::string, size_t>& groupNameToIdMap);
	std::string getProblemType(EntityBase* solverEntity);
	std::string readOutputFile(const std::string &fileName);
	std::string getUniqueTempDir(void);
	std::string getSystemTempDir(void);
	std::string readEnvironmentVariable(const std::string &variableName);
	bool checkFileOrDirExists(const std::string &path);
	bool deleteDirectory(const std::string &pathName);
	void readMeshItemInfo(ot::UID meshDataID, std::map<ot::UID, ot::EntityInformation>& meshItemInfo);
	void readMaterialProperties(std::map<std::string, EntityProperties>& materialProperties);

	Application *application;
};
