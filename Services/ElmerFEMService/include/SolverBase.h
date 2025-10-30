// @otlicense
// File: SolverBase.h
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

#include "OTCore/TypeNames.h"
#include "EntityProperties.h"
#include "EntityInformation.h"

#include <fstream>
#include <map>
#include <list>
#include <string>
#include <sstream>
#include <windows.h> 

class EntityBase;
class Application;

namespace ot {
	namespace components {
		class UiComponent;
	}
}

class SolverBase {
public:
	SolverBase() : solverEntity(nullptr) {}
	virtual ~SolverBase() {}

	void setData(EntityBase* _solverEntity, const std::string &_meshDataName, std::map<ot::UID, ot::EntityInformation>& _meshItemInfo,
				 std::map<ot::UID, EntityProperties>& _entityProperties, std::map<std::string, size_t>& _groupNameToIdMap,
				 std::map<std::string, EntityProperties>& _materialProperties);

	virtual void writeInputFile(std::ofstream& controlFile, Application *app) = 0;
	virtual std::string runSolver(const std::string& tempDirPath, ot::components::UiComponent* uiComponent) = 0;
	virtual void convertResults(const std::string& tempDirPath, Application* app, EntityBase *solverEntity) = 0;

	void runSolverExe(const std::string& inputFileName, const std::string& solvTarget, const std::string& postTarget, const std::string& workingDirectory, ot::components::UiComponent* uiComponent);

protected:
	bool runExecutableAndWaitForCompletion(std::string commandLine, std::string workingDirectory, ot::components::UiComponent* uiComponent);
	std::string readEnvironmentVariable(const std::string& variableName);
	bool isPECMaterial(const std::string& materialName);
	void ReadFromPipe(HANDLE g_hChildStd_OUT_Rd, ot::components::UiComponent* uiComponent);

	EntityBase* solverEntity;
	std::string meshDataName;
	std::map<ot::UID, ot::EntityInformation> meshItemInfo;
	std::map<ot::UID, EntityProperties> entityProperties;
	std::map<std::string, size_t> groupNameToIdMap;
	std::map<std::string, EntityProperties> materialProperties;

	std::stringstream solverOutput;
};
