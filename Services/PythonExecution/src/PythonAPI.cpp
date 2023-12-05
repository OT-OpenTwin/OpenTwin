/*****************************************************************//**
 * \file   PythonAPI.cpp
 * \brief  Layer that shall be used from the application.cpp. This class deals with the workflow of loading python script entities, executing them and 
 *			sending the results back to the service that requested the execution.
 * 
 * \author Wagner
 * \date   July 2023
 *********************************************************************/
#include "PythonAPI.h"
#include "Application.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "ClassFactory.h"

#include "PythonLoadedModules.h"
#include "PythonModuleAPI.h"
#include "EntityBuffer.h"

PythonAPI::PythonAPI()
{
	_wrapper.InitializePythonInterpreter();
}

ot::ReturnValues PythonAPI::Execute(std::list<std::string>& scripts, std::list<std::list<ot::Variable>>& parameterSet)
{
	EnsureScriptsAreLoaded(scripts);
	EntityBuffer::INSTANCE().setModelComponent(Application::instance()->modelComponent());
	auto currentParameterSet = parameterSet.begin();
	
	PythonObjectBuilder pyObBuilder;
	ot::ReturnValues returnValues;
	for (std::string& scriptName : scripts)
	{
		std::string moduleName = PythonLoadedModules::INSTANCE()->getModuleName(scriptName).value();
		std::string entryPoint = _moduleEntrypointByScriptName[scriptName];

		std::list<ot::Variable>& parameterSetForScript = *currentParameterSet;
		CPythonObjectNew pythonParameterSet(nullptr);
		if (parameterSetForScript.size() != 0)
		{
			pythonParameterSet.reset(pyObBuilder.setVariableList(parameterSetForScript));
		}
		CPythonObjectNew pReturnValue = _wrapper.ExecuteFunction(entryPoint, pythonParameterSet, moduleName);

		returnValues.addData(scriptName, pyObBuilder.getVariableList(pReturnValue));
		currentParameterSet++;
	}
	return returnValues;
}

void PythonAPI::EnsureScriptsAreLoaded(std::list<std::string> scripts)
{

	scripts.unique();
	std::list<ot::EntityInformation> entityInfos;
	auto modelComponent = Application::instance()->modelComponent();
	modelComponent->getEntityInformation(scripts, entityInfos);
	
	if (entityInfos.size() != scripts.size())
	{
		std::string missingScripts = "";
		for (std::string& scriptName : scripts)
		{
			bool found = false;
			for (auto& entity : entityInfos)
			{
				if (entity.getName() == scriptName) { found = true; break; }
			}
			if (!found) { missingScripts += scriptName + ", "; }
		}
		missingScripts = missingScripts.substr(0, missingScripts.size() - 2);
		throw std::exception(("Python execution aborted since the following scripts were not found: " + missingScripts).c_str());
	}

	Application::instance()->prefetchDocumentsFromStorage(entityInfos);
	for (auto& entityInfo : entityInfos)
	{
		std::string scriptName = entityInfo.getName();
		std::optional<std::string> moduleName = PythonLoadedModules::INSTANCE()->getModuleName(scriptName);
		if (!moduleName.has_value())
		{
			LoadScipt(entityInfo);
		}
		else
		{
			std::string shouldModuleName = PythonLoadedModules::INSTANCE()->GetModuleName(entityInfo);
			if (moduleName.value() != shouldModuleName) // Script with same name but different ID or Version
			{
				PythonLoadedModules::INSTANCE()->RemoveModule(scriptName);
				_moduleEntrypointByScriptName.erase(scriptName);
				LoadScipt(entityInfo);
			}
		}
	}
}

void PythonAPI::LoadScipt(ot::EntityInformation& entityInformation)
{
	auto modelComponent = Application::instance()->modelComponent();
	ClassFactory classFactory;
	auto baseEntity = modelComponent->readEntityFromEntityIDandVersion(entityInformation.getID(), entityInformation.getVersion(), classFactory);
	std::unique_ptr<EntityFile> script(dynamic_cast<EntityFile*>(baseEntity));
	auto plainData = script->getData()->getData();
	std::string execution(plainData.begin(), plainData.end());

	std::string moduleName = PythonLoadedModules::INSTANCE()->AddModuleForEntity(baseEntity);
	_wrapper.Execute(execution, moduleName);
	PythonModuleAPI moduleAPI;
	_moduleEntrypointByScriptName[script->getName()] = moduleAPI.GetModuleEntryPoint(moduleName);
}

