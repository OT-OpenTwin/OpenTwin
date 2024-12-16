/*****************************************************************//**
 * \file   PythonAPI.cpp
 * \brief  Layer that shall be used from the application.cpp. This class deals with the workflow of loading python script entities, executing them and 
 *			sending the results back to the service that requested the execution.
 * 
 * \author Wagner
 * \date   July 2023
 *********************************************************************/
#include "PythonAPI.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "ClassFactory.h"
#include "Application.h"

#include "PythonLoadedModules.h"
#include "PythonModuleAPI.h"
#include "EntityBuffer.h"
#include "EntityFileText.h"

PythonAPI::PythonAPI()
{
	m_wrapper.InitializePythonInterpreter();
}

ot::ReturnValues PythonAPI::execute(std::list<std::string>& _scripts, std::list<std::list<ot::Variable>>& _parameterSet)
{
	assert(_scripts.size() == _parameterSet.size());
	std::list<ot::EntityInformation> scriptEntities = ensureScriptsAreLoaded(_scripts);
	EntityBuffer::instance().setModelServiceAPI(&Application::instance().getModelServiceAPI());
	auto currentParameterSet = _parameterSet.begin();
	
	PythonObjectBuilder pyObBuilder;
	ot::ReturnValues returnValues;
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
			CPythonObjectNew pReturnValue = m_wrapper.ExecuteFunction(entryPoint, pythonParameterSet, moduleName);
			returnValues.addData(scriptEntity.getEntityName(), pyObBuilder.getGenericDataStructList(pReturnValue));
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
	return returnValues;
}

ot::ReturnValues PythonAPI::execute(const std::string& command) noexcept(false)
{
	std::string moduleName = std::to_string(EntityBase::getUidGenerator()->getUID());
	CPythonObjectNew pReturnValue = m_wrapper.execute(command,moduleName);
	ot::ReturnValues returnValues;
	PythonObjectBuilder pyObBuilder;
	returnValues.addData("", pyObBuilder.getGenericDataStructList(pReturnValue));
	return returnValues;
}

std::list<ot::EntityInformation> PythonAPI::ensureScriptsAreLoaded(const std::list<std::string>& _scripts)
{
	//First we get the information of the used scripts
	std::list<std::string> uniqueScriptNames = _scripts;
	uniqueScriptNames.sort();
	uniqueScriptNames.unique();
	std::list<ot::EntityInformation> entityInfos;
	auto modelComponent = Application::instance().getModelServiceAPI();
	modelComponent.getEntityInformation(uniqueScriptNames, entityInfos);
	
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

void PythonAPI::loadScipt(const ot::EntityInformation& _entityInformation)
{
	try
	{
		ot::ModelServiceAPI& modelComponent = Application::instance().getModelServiceAPI();
		EntityBase* baseEntity = modelComponent.readEntityFromEntityIDandVersion(_entityInformation.getEntityID(), _entityInformation.getEntityVersion(), Application::instance().getClassFactory());
		std::unique_ptr<EntityFileText> script(dynamic_cast<EntityFileText*>(baseEntity));
		std::string execution = script->getText();

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

