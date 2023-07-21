#include "PythonAPI.h"
#include "Application.h"
#include "OpenTwinFoundation/ModelComponent.h"
#include "ClassFactory.h"

#include "PythonLoadedModules.h"
#include "PythonModuleAPI.h"
#include "EntityBuffer.h"

PythonAPI::PythonAPI()
{
	//PythonWrapper::INSTANCE()->InitializePythonInterpreter();
	_wrapper.InitializePythonInterpreter();
}

std::list<variable_t> PythonAPI::Execute(std::list<std::string>& scripts, std::list<std::optional<std::list<variable_t>>>& parameterSet)
{
	EnsureScriptsAreLoaded(scripts);
	EntityBuffer::INSTANCE().setModelComponent(Application::instance()->modelComponent());
	auto currentParameterSet = parameterSet.begin();
	std::list<variable_t> returnValues;
	PythonObjectBuilder pyObBuilder;
	for (std::string& scriptName : scripts)
	{
		std::string moduleName = PythonLoadedModules::INSTANCE()->getModuleName(scriptName).value();
		std::string entryPoint = _moduleEntrypointByScriptName[scriptName];

		std::optional <std::list<variable_t>>& parameterSetForScript = *currentParameterSet;
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
	Application::instance()->prefetchDocumentsFromStorage(entityInfos);
	ClassFactory classFactory;

	for (auto& entityInfo : entityInfos)
	{
		std::string scriptName = entityInfo.getName();
		std::optional<std::string> moduleName = PythonLoadedModules::INSTANCE()->getModuleName(scriptName);
		if (!moduleName.has_value())
		{

			auto baseEntity = modelComponent->readEntityFromEntityIDandVersion(entityInfo.getID(), entityInfo.getVersion(), classFactory);
			std::unique_ptr<EntityParameterizedDataSource> script(dynamic_cast<EntityParameterizedDataSource*>(baseEntity));
			script->loadData();
			auto plainData = script->getData()->getData();
			std::string execution(plainData.begin(), plainData.end());
			
			moduleName = PythonLoadedModules::INSTANCE()->AddModuleForEntity(baseEntity);
			_wrapper.Execute(execution, moduleName.value());
			PythonModuleAPI moduleAPI;

			_moduleEntrypointByScriptName[scriptName] = moduleAPI.GetModuleEntryPoint(moduleName.value());
		}
	}
}

CPythonObjectNew PythonAPI::CreateParameterSet(std::list<variable_t>& parameterSet)
{
	PythonObjectBuilder pObjectBuilder;
	pObjectBuilder.StartTupleAssemply(parameterSet.size());

	for (variable_t& parameter : parameterSet)
	{
		if (std::holds_alternative<int32_t>(parameter))
		{
			auto value = pObjectBuilder.setInt32(std::get<int32_t>(parameter));
			pObjectBuilder << &value;
		}
		else if (std::holds_alternative<int64_t>(parameter))
		{
			auto value = pObjectBuilder.setInt32(static_cast<int32_t>(std::get<int64_t>(parameter)));
			pObjectBuilder << &value;
		}
		else if (std::holds_alternative<double>(parameter))
		{
			auto value = pObjectBuilder.setDouble(std::get<double>(parameter));
			pObjectBuilder << &value;
		}
		else if (std::holds_alternative<float>(parameter))
		{
			auto value = pObjectBuilder.setDouble(static_cast<double>(std::get<float>(parameter)));
			pObjectBuilder << &value;
		}
		else if (std::holds_alternative<const char*>(parameter))
		{
			auto value = pObjectBuilder.setString(std::string(std::get<const char*>(parameter)));
			pObjectBuilder << &value;
		}
		else if (std::holds_alternative<bool>(parameter))
		{
			auto value = pObjectBuilder.setBool(std::get<bool>(parameter));
			pObjectBuilder << &value;
		}
		else
		{
			throw std::exception("Type is not supported by the python service.");
		}
	}
	return pObjectBuilder.getAssembledTuple();
}

