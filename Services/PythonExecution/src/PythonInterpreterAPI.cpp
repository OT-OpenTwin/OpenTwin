// @otlicense
// File: PythonInterpreterAPI.cpp
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

#include "PythonInterpreterAPI.h"
#include "Application.h"
#include "EntityAPI.h"

#include "PythonLoadedModules.h"
#include "PythonModuleAPI.h"
#include "EntityBuffer.h"
#include "EntityFileText.h"
#include "DataBuffer.h"
#include "PackageHandler.h"

#include <filesystem>

void PythonInterpreterAPI::initializeEnvironment(const std::string& _environmentName)
{
	//Pyrit next to a custom environment or instead ?
	if (_environmentName == "Pyrit")
	{
		// Environment is a special, which already exists in the python interpreter installation
		OT_LOG_D("Initialize Pyrit environment");
		m_wrapper.initializePythonInterpreter("PyritEnvironment");
		//m_wrapper.initializePythonInterpreter(_environmentName);
	}
}

void PythonInterpreterAPI::execute(std::list<std::string>& _scripts, std::list<std::list<ot::Variable>>& _parameterSet)
{
	assert(_scripts.size() == _parameterSet.size());
	std::list<ot::EntityInformation> scriptEntities = ensureScriptsAreLoaded(_scripts);
	auto currentParameterSet = _parameterSet.begin();
	
	PythonObjectBuilder pyObBuilder;
	EntityBuffer::instance().clearBuffer();// Entities and properties are buffered by name. It needs to be cleared, so that no outdated entities are accessed in the next execution.
	
	for (ot::EntityInformation& scriptEntity : scriptEntities)
	{
		try
		{
			std::string moduleName = PythonLoadedModules::instance().getModuleName(scriptEntity).value();
			std::string entryPoint = m_moduleEntrypointByModuleName[moduleName];

			std::list<ot::Variable>& parameterSetForScript = *currentParameterSet;
			CPythonObjectNew pythonParameterSet(nullptr);
			if (parameterSetForScript.size() != 0)
			{
				pythonParameterSet.reset(pyObBuilder.setVariableTuple(parameterSetForScript));
			}

			OT_LOG_D("Execute script " + scriptEntity.getEntityName());
			CPythonObjectNew pReturnValue = m_wrapper.executeFunction(entryPoint, pythonParameterSet, moduleName);
			try
			{
				if (pReturnValue)
				{
					const std::string returnString = pyObBuilder.getStringValue(pReturnValue, "");
					DataBuffer::instance().addReturnData(scriptEntity.getEntityName(),returnString);
				}
			}
			catch (std::exception&){}
			currentParameterSet++;
			OT_LOG_D("Script execution succeeded");
		}
		catch (const std::exception& e)
		{
			const std::string message = "Script " + scriptEntity.getEntityName() + " failed in execution due to: " + std::string(e.what());
			throw std::exception(message.c_str());
		}
	}
	EntityBuffer::instance().saveChangedEntities();
}

void PythonInterpreterAPI::execute(const std::string& command) noexcept(false)
{
	std::string moduleName = std::to_string(EntityBase::getUidGenerator()->getUID());
	CPythonObjectNew pReturnValue = m_wrapper.execute(command,moduleName);
	ot::ReturnValues returnValues;
	PythonObjectBuilder pyObBuilder;
	try
	{
		const std::string returnString = pyObBuilder.getStringValue(pReturnValue, "");
		DataBuffer::instance().addReturnData("", returnString);
	}
	catch (std::exception&) { 
	}
}

std::list<ot::EntityInformation> PythonInterpreterAPI::ensureScriptsAreLoaded(const std::list<std::string>& _scripts)
{
	//First we get the information of the used scripts
	std::list<std::string> uniqueScriptNames = _scripts;
	uniqueScriptNames.sort();
	uniqueScriptNames.unique();
	std::list<ot::EntityInformation> entityInfos;
	ot::ModelServiceAPI::getEntityInformation(uniqueScriptNames, entityInfos);
	
	std::map<std::string, ot::EntityInformation> entityInfosByName;
	for (const ot::EntityInformation& entityInfo : entityInfos)
	{
		entityInfosByName[entityInfo.getEntityName()] = entityInfo;
	}
 
	//If some of the requested scripts are not existend in the current model state, we end the execution here
	if (entityInfos.size() != uniqueScriptNames.size())
	{
		std::string missingScripts = "";
		for (std::string& scriptName : uniqueScriptNames)
		{
			auto entityInfoByName =	entityInfosByName.find(scriptName);

			if (entityInfoByName == entityInfosByName.end())
			{ 
				missingScripts += scriptName + ", "; 
			}
		}
		missingScripts = missingScripts.substr(0, missingScripts.size() - 2);
		throw std::exception(("scripts were not found: " + missingScripts).c_str());
	}
	
	//Now we load all scripts
	Application::instance().prefetchDocumentsFromStorage(entityInfos);
	for (ot::EntityInformation& entityInfo : entityInfos)
	{
		OT_LOG_D("Loading script " + entityInfo.getEntityName());
		loadScipt(entityInfo);
	}

	//Now we build up a list of the execution scripts but with their entityInformation instead of their plain names
	std::list<ot::EntityInformation> scriptEntityInfoList;
	for (const std::string& scriptName : _scripts)
	{
		scriptEntityInfoList.push_back(entityInfosByName[scriptName]);
	}
	assert(scriptEntityInfoList.size() == entityInfos.size());
	return scriptEntityInfoList;
}

void PythonInterpreterAPI::loadScipt(const ot::EntityInformation& _entityInformation)
{
	try
	{
		EntityBase* baseEntity = ot::EntityAPI::readEntityFromEntityIDandVersion(_entityInformation.getEntityID(), _entityInformation.getEntityVersion());
		std::unique_ptr<EntityFileText> script(dynamic_cast<EntityFileText*>(baseEntity));
		std::string execution = script->getText();

		PackageHandler packageHandler;
		packageHandler.setTargetPath(m_wrapper.getEnvironmentPath());
		packageHandler.importMissingPackages(execution);

		//First we add a module for the script execution. This way there won't be any namespace conflicts between the scripts since they are all executed in the same namespace
		const std::string moduleName = PythonLoadedModules::instance().addModuleForEntity(_entityInformation);
		if (moduleName == "")
		{
			const std::string message = "failed to determine a module name for script: " + _entityInformation.getEntityName();
			throw std::exception(message.c_str());
		}
		
		//Executing the script in this module will only add the contained functions to the module. No return value expected. Error handling by the wrapper
		auto result = m_wrapper.execute(execution, moduleName);
		
		//Since potentially multiple functions exist in a module, an entry point needs to be explicitly defined
		PythonModuleAPI moduleAPI;
		const std::string entryPoint = moduleAPI.getModuleEntryPoint(moduleName);
		OT_LOG_D("Determined entry point for " + _entityInformation.getEntityName() + " to be: " + entryPoint);
		m_moduleEntrypointByModuleName[moduleName] = entryPoint;
	}
	catch (std::exception& e)
	{
		const std::string message = "Failed to load script: " + _entityInformation.getEntityName() + " due to: " + e.what();
		throw std::exception(message.c_str());
	}
}

