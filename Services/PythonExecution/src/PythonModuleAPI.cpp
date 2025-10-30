// @otlicense
// File: PythonModuleAPI.cpp
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

#include "PythonModuleAPI.h"
#include "PythonObjectBuilder.h"
#include "PythonLoadedModules.h"

const PythonModuleAPI& PythonModuleAPI::instance(void) {
	static PythonModuleAPI g_instance;
	return g_instance;
}

std::string PythonModuleAPI::getModuleEntryPoint(const std::string& _moduleName) const
{

	CPythonObjectBorrowed pyhtonModule(PyImport_ImportModule(_moduleName.c_str()));
	if (pyhtonModule == nullptr)
	{
		const std::string entityName =	PythonLoadedModules::instance().getEntityName(_moduleName);
		const std::string message = "Failed to import module of entity " + entityName;
		throw std::exception(message.c_str());
	}
	
	if (!hasScriptHasEntryPoint(pyhtonModule))
	{
		const std::string entityName = PythonLoadedModules::instance().getEntityName(_moduleName);
		const std::string message = "Script " + entityName + " has no entry point. Each script in OpenTwin requires a function: " + m_defaultEntryPoint;
		throw std::exception(message.c_str());
	}

	return m_defaultEntryPoint;
}

std::string PythonModuleAPI::getModuleEntryPoint(CPythonObject& _module) const
{
	CPythonObjectBorrowed globalDictionary = PyModule_GetDict(_module);
	PythonObjectBuilder pythonObjectBuilder;
	CPythonObjectBorrowed allGlobalDictItems = pythonObjectBuilder.getDictItem(globalDictionary);

	bool firstOfFunctionNames = false;
	std::string entryPointName = "";

	for (int i = 0; i < PyList_Size(allGlobalDictItems); i++)
	{
		CPythonObjectBorrowed listEntry = pythonObjectBuilder.getListItem(allGlobalDictItems, i);
		std::string listEntryName = pythonObjectBuilder.getStringValueFromTuple(listEntry, 0, "ListValue");


		if (listEntryName == "__builtins__") //Last default entry
		{
			firstOfFunctionNames = true;
		}
		else if (firstOfFunctionNames)
		{
			CPythonObjectBorrowed listElement(PyDict_GetItemString(globalDictionary, listEntryName.c_str()));

			if (PyFunction_Check(listElement))
			{
				if (entryPointName == "")
				{
					entryPointName = listEntryName;
				}
				else
				{
					entryPointName = m_defaultEntryPoint;
					break;
				}
			}
		}
	}
	return entryPointName;
}

bool PythonModuleAPI::hasScriptHasEntryPoint(const CPythonObject& _module) const
{
	CPythonObjectBorrowed globalDictionary = PyModule_GetDict(_module);
	PythonObjectBuilder pythonObjectBuilder;
	CPythonObjectBorrowed allGlobalDictItems = pythonObjectBuilder.getDictItem(globalDictionary);

	bool foundEntryPoint = false;
	std::string entryPointName = "";

	for (int i = 0; i < PyList_Size(allGlobalDictItems); i++)
	{
		CPythonObjectBorrowed listEntry = pythonObjectBuilder.getListItem(allGlobalDictItems, i);
		std::string listEntryName = pythonObjectBuilder.getStringValueFromTuple(listEntry, 0, "ListValue");


		if (listEntryName == m_defaultEntryPoint) 
		{
			foundEntryPoint= true;
			break;
		}
	}
	return foundEntryPoint;
}
