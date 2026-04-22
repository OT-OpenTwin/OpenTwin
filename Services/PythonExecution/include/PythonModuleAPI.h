// @otlicense
// File: PythonModuleAPI.h
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

//! @brief  Functions that investigate an existing module for the starting function. If the module contains only one funtion, it will be the starting function of the module. 
//!			If the module holds multiple function definitions it requires one function called "__main__", which will be the starting function of the module.

#pragma once
#include <string>
#include <map>
#include "CPythonObject.h"
class PythonModuleAPI
{
public:
	static PythonModuleAPI& instance(void);

	void setEntryPoint(const std::string& _moduleName, const std::string& _entryPoint);

	std::string getModuleEntryPoint(const std::string& _moduleName) const;
	std::string getModuleEntryPoint(CPythonObject& _module) const;

private:
	PythonModuleAPI() = default;
	std::string m_defaultEntryPoint = "__main__";
	std::map<std::string, std::string> m_entryPointByModuleName;
	bool hasScriptHasEntryPoint(const CPythonObject& _module, const std::string& _moduleName) const;
};

