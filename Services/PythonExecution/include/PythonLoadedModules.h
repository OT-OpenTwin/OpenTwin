// @otlicense
// File: PythonLoadedModules.h
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
#include <string>
#include <set>
#include <map>
#include <optional>

#include "EntityInformation.h"
#include "OTCore/LogDispatcher.h"

//! @brief Singleton that holds a map of all modules that were loaded in the python interpreter, sorted by the name of the corresponding script name.
class PythonLoadedModules
{
public:
	static PythonLoadedModules& instance(void);
	
	std::optional<std::string> getModuleName(const ot::EntityInformation& _scriptEntityInfos);
	std::string addModuleForEntity(const ot::EntityInformation& _scriptEntityInfo);
	std::string createUniqueModuleName(const ot::EntityInformation& _entityInfo);
	
	std::string getEntityName(const std::string& _moduleName);
private:

	PythonLoadedModules() {};
	std::set<std::string> m_moduleNames;
	std::map<std::string, std::string> m_entityNameByModuleName;
};
