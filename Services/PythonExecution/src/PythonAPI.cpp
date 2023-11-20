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

std::list<ot::Variable> PythonAPI::Execute(std::list<std::string>& scripts, std::list<std::optional<std::list<ot::Variable>>>& parameterSet)
{
	EnsureScriptsAreLoaded(scripts);
	EntityBuffer::INSTANCE().setModelComponent(Application::instance()->modelComponent());
	auto currentParameterSet = parameterSet.begin();
	std::list<ot::Variable> returnValues;
	PythonObjectBuilder pyObBuilder;
	for (std::string& scriptName : scripts)
	{
		
		std::string moduleName = PythonLoadedModules::INSTANCE()->getModuleName(scriptName).value();
		std::string entryPoint = _moduleEntrypointByScriptName[scriptName];

		std::optional <std::list<ot::Variable>>& parameterSetForScript = *currentParameterSet;
		CPythonObjectNew pythonParameterSet(nullptr);
		if (parameterSetForScript.has_value())
		{
			pythonParameterSet.reset(CreateParameterSet(parameterSetForScript.value()));
		}
		CPythonObjectNew returnValue = _wrapper.ExecuteFunction(entryPoint, pythonParameterSet, moduleName);

		auto vReturnValue = pyObBuilder.getVariable(returnValue);
		if (vReturnValue.has_value())
		{
			returnValues.emplace_front(vReturnValue.value());
		}
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
	ClassFactory classFactory;

	for (auto& entityInfo : entityInfos)
	{
		std::string scriptName = entityInfo.getName();
		std::optional<std::string> moduleName = PythonLoadedModules::INSTANCE()->getModuleName(scriptName);
		if (!moduleName.has_value())
		{

			auto baseEntity = modelComponent->readEntityFromEntityIDandVersion(entityInfo.getID(), entityInfo.getVersion(), classFactory);
			std::unique_ptr<EntityFile> script(dynamic_cast<EntityFile*>(baseEntity));
			auto plainData = script->getData()->getData();
			std::string execution(plainData.begin(), plainData.end());

			moduleName = PythonLoadedModules::INSTANCE()->AddModuleForEntity(baseEntity);
			_wrapper.Execute(execution, moduleName.value());
			PythonModuleAPI moduleAPI;

			_moduleEntrypointByScriptName[scriptName] = moduleAPI.GetModuleEntryPoint(moduleName.value());
		}
	}
}

CPythonObjectNew PythonAPI::CreateParameterSet(std::list<ot::Variable>& parameterSet)
{
	PythonObjectBuilder pObjectBuilder;
	pObjectBuilder.StartTupleAssemply(static_cast<int>(parameterSet.size()));

	for (ot::Variable& parameter : parameterSet)
	{
		if (parameter.isInt32())
		{
			auto value = pObjectBuilder.setInt32(parameter.getInt32());
			pObjectBuilder << &value;
		}
		else if (parameter.isInt64())
		{
			auto value = pObjectBuilder.setInt32(static_cast<int32_t>(parameter.getInt64()));
			pObjectBuilder << &value;
		}
		else if (parameter.isDouble())
		{
			auto value = pObjectBuilder.setDouble(parameter.getDouble());
			pObjectBuilder << &value;
		}
		else if (parameter.isFloat())
		{
			auto value = pObjectBuilder.setDouble(static_cast<double>(parameter.getFloat()));
			pObjectBuilder << &value;
		}
		else if (parameter.isConstCharPtr())
		{
			auto value = pObjectBuilder.setString(std::string(parameter.getConstCharPtr()));
			pObjectBuilder << &value;
		}
		else if (parameter.isBool())
		{
			auto value = pObjectBuilder.setBool(parameter.getBool());
			pObjectBuilder << &value;
		}
		else
		{
			throw std::exception("Type is not supported by the python service.");
		}
	}
	return pObjectBuilder.getAssembledTuple();
}

