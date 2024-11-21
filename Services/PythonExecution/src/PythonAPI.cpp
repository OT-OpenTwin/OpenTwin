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

PythonAPI::PythonAPI()
{
	_wrapper.InitializePythonInterpreter();
}

ot::ReturnValues PythonAPI::Execute(std::list<std::string>& scripts, std::list<std::list<ot::Variable>>& parameterSet)
{
	std::list<ot::EntityInformation> scriptEntities = EnsureScriptsAreLoaded(scripts);
	EntityBuffer::INSTANCE().setModelServiceAPI(&Application::instance()->ModelServiceAPI());
	auto currentParameterSet = parameterSet.begin();
	
	PythonObjectBuilder pyObBuilder;
	ot::ReturnValues returnValues;
	for (ot::EntityInformation& scriptEntity : scriptEntities)
	{
		std::string moduleName = PythonLoadedModules::INSTANCE()->getModuleName(scriptEntity).value();
		std::string entryPoint = _moduleEntrypointByModuleName[moduleName];

		std::list<ot::Variable>& parameterSetForScript = *currentParameterSet;
		CPythonObjectNew pythonParameterSet(nullptr);
		if (parameterSetForScript.size() != 0)
		{
			pythonParameterSet.reset(pyObBuilder.setVariableTuple(parameterSetForScript));
		}

		OT_LOG_D("Execute script " + scriptEntity.getEntityName());
		CPythonObjectNew pReturnValue = _wrapper.ExecuteFunction(entryPoint, pythonParameterSet, moduleName);
		returnValues.addData(scriptEntity.getEntityName(), pyObBuilder.getGenericDataStructList(pReturnValue));
		currentParameterSet++;
		OT_LOG_D("Script execution succeeded");
		EntityBuffer::INSTANCE().ClearBuffer();// Entities and properties are buffered by name. It needs to be cleared, so that no outdated entities are accessed in the next execution.
	}
	return returnValues;
}

ot::ReturnValues PythonAPI::Execute(const std::string& command) noexcept(false)
{
	std::string moduleName = std::to_string(EntityBase::getUidGenerator()->getUID());
	CPythonObjectNew pReturnValue = _wrapper.Execute(command,moduleName);
	ot::ReturnValues returnValues;
	PythonObjectBuilder pyObBuilder;
	returnValues.addData("", pyObBuilder.getGenericDataStructList(pReturnValue));
	return returnValues;
}

std::list<ot::EntityInformation> PythonAPI::EnsureScriptsAreLoaded(std::list<std::string> scripts)
{
	scripts.unique();
	std::list<ot::EntityInformation> entityInfos;
	auto modelComponent = Application::instance()->ModelServiceAPI();
	modelComponent.getEntityInformation(scripts, entityInfos);
	
	if (entityInfos.size() != scripts.size())
	{
		std::string missingScripts = "";
		for (std::string& scriptName : scripts)
		{
			bool found = false;
			for (auto& entity : entityInfos)
			{
				if (entity.getEntityName() == scriptName) { found = true; break; }
			}
			if (!found) { missingScripts += scriptName + ", "; }
		}
		missingScripts = missingScripts.substr(0, missingScripts.size() - 2);
		throw std::exception(("Python execution aborted since the following scripts were not found: " + missingScripts).c_str());
	}
		
	std::list<ot::EntityInformation> scriptsNotLoadedYet;
	for (auto& entityInfo : entityInfos)
	{
		const std::string scriptName = entityInfo.getEntityName();
		OT_LOG_D("Loading script " + scriptName);
		std::optional<std::string> moduleName = PythonLoadedModules::INSTANCE()->getModuleName(entityInfo);
		if (!moduleName.has_value())
		{
			scriptsNotLoadedYet.push_back(entityInfo);
		}
	}

	if (scriptsNotLoadedYet.size() != 0)
	{
		Application::instance()->prefetchDocumentsFromStorage(scriptsNotLoadedYet);
		for (ot::EntityInformation& entityInfo : scriptsNotLoadedYet)
		{
			LoadScipt(entityInfo);
		}
	}

	if (scripts.size() == entityInfos.size())
	{
		return entityInfos;
	}
	else
	{
		std::list<ot::EntityInformation> scriptEntityInfoList;
		for (const std::string& scriptName : scripts)
		{
			for (ot::EntityInformation& entityInfo : entityInfos)
			{
				if (entityInfo.getEntityName() == scriptName)
				{
					scriptEntityInfoList.push_back(entityInfo);
					break;
				}
			}
		}
		assert(scriptEntityInfoList.size() == entityInfos.size());
		return scriptEntityInfoList;
	}
}

void PythonAPI::LoadScipt(ot::EntityInformation& entityInformation)
{
	auto modelComponent = Application::instance()->ModelServiceAPI();
	auto baseEntity = modelComponent.readEntityFromEntityIDandVersion(entityInformation.getEntityID(), entityInformation.getEntityVersion(), Application::instance()->getClassFactory());
	std::unique_ptr<EntityFile> script(dynamic_cast<EntityFile*>(baseEntity));
	auto plainData = script->getData()->getData();
	std::string execution(plainData.begin(), plainData.end());

	std::string moduleName = PythonLoadedModules::INSTANCE()->AddModuleForEntity(entityInformation);
	_wrapper.Execute(execution, moduleName);
	PythonModuleAPI moduleAPI;
	_moduleEntrypointByModuleName[moduleName] = moduleAPI.GetModuleEntryPoint(moduleName);
}

