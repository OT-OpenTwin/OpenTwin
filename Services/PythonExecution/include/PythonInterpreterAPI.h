// @otlicense
// File: PythonInterpreterAPI.h
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
#include <optional>
#include <list>
#include "PythonWrapper.h"
#include "EntityFile.h"
#include "PythonObjectBuilder.h"
#include "EntityInformation.h"
#include "OTCore/ReturnValues.h"
#include "PackageHandler.h"
#include "InterpreterPathSettings.h"
#include "WorkerWaiterState.h"
#include "EnvironmentsGarbageCollector.h"

//! @brief This class deals with the workflow of loading python script entities, executing them and sending the results back to the service that requested the execution.
class PythonInterpreterAPI
{
	friend class FixturePythonAPI;
public:
	//! @brief Initialise an environment described by a manifest entity
	//! @param _environmentName 
	void initializeEnvironment(ot::UID _manifestEntityUID);
	void initializeEnvironment(const std::string& _environmentName);
	void checkEnvironmentIsInitialised(ot::UID _manifestEntityUID);
	void execute(std::list<std::string>& _scripts, std::list<std::list<ot::Variable>>& _parameterSet) noexcept(false);
	void execute(const std::string& _command) noexcept(false);

	void cleanup();
private:
	std::map<std::string , std::string> m_moduleEntrypointByModuleName;
	PythonWrapper m_wrapper;
	PackageHandler m_packageHandler;
	WorkerWaiterState m_workerWaiterState;
	EnvironmentsGarbageCollector m_garbageCollector;
	InterpreterPathSettings m_interpreterPathSettings;

	std::list<ot::EntityInformation> ensureScriptsAreLoaded(const std::list<std::string>& _scripts);
	std::string loadScipt(const ot::EntityInformation& _entityInformation);
	void addScriptAsModule(const std::string _execution, const ot::EntityInformation& _entityInformation);
};
