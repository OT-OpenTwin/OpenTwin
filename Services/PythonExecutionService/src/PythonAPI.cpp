#include "PythonAPI.h"
#include "Application.h"
#include "OpenTwinFoundation/ModelComponent.h"
#include "ClassFactory.h"

#include "PythonLoadedModules.h"

PythonAPI::PythonAPI()
{
	_wrapper.InitializePythonInterpreter();
}

std::list<variable_t> PythonAPI::Execute(ot::UIDList& scripts, std::list<std::list<variable_t>>& parameterSet)
{
	EnsureScriptsAreLoaded(scripts);

	auto currentParameterSet = parameterSet.begin();
	std::list<variable_t> returnValues;
	PythonObjectBuilder pyObBuilder;
	for (ot::UID& scriptUID : scripts)
	{
		std::list<variable_t>& parameterSetForScript = *currentParameterSet;
		CPythonObjectNew parameterSet = CreateParameterSet(parameterSetForScript);
		
		std::string scriptName = _scriptNameByUID[scriptUID];
		std::string moduleName = (*PythonLoadedModules::INSTANCE()->getModuleNamesByScriptName())[scriptName];
		std::string entryPoint = _moduleEntrypointByScriptName[scriptName];

		CPythonObjectNew returnValue = _wrapper.ExecuteFunction(entryPoint, parameterSet, moduleName);
		returnValues.emplace_front(pyObBuilder.getVariable(returnValue));
	}
		
	return returnValues;
}



void PythonAPI::EnsureScriptsAreLoaded(ot::UIDList& scripts)
{
	scripts.unique();
	Application::instance()->prefetchDocumentsFromStorage(scripts);
	std::list<ot::EntityInformation> entityInfos;
	auto modelComponent = Application::instance()->modelComponent();
	modelComponent->getEntityInformation(scripts, entityInfos);
	ClassFactory classFactory;
	auto moduleNameByScriptName = PythonLoadedModules::INSTANCE()->getModuleNamesByScriptName();
	for (auto& entityInfo : entityInfos)
	{
		std::string scriptName = entityInfo.getName();
		if (moduleNameByScriptName->find(scriptName) != moduleNameByScriptName->end())
		{
			ot::UID uid = entityInfo.getID();
			ot::UID version = entityInfo.getVersion();
			std::string moduleName = std::to_string(uid) + std::to_string(version);

			auto baseEntity = modelComponent->readEntityFromEntityIDandVersion(entityInfo.getID(), entityInfo.getVersion(), classFactory);
			std::unique_ptr<EntityParameterizedDataSource> script(dynamic_cast<EntityParameterizedDataSource*>(baseEntity));
			auto plainData = script->getData()->getData();
			std::string execution(plainData.begin(), plainData.end());
			_wrapper.Execute(execution, moduleName);
			(*moduleNameByScriptName)[scriptName] = moduleName;
			_scriptNameByUID[uid] = scriptName;
			_moduleEntrypointByScriptName[scriptName] = GetModuleEntryPoint(moduleName);
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
			pObjectBuilder << pObjectBuilder.setInt32(std::get<int32_t>(parameter));
		}
		else if (std::holds_alternative<int64_t>(parameter))
		{
			pObjectBuilder << pObjectBuilder.setInt32(static_cast<int32_t>(std::get<int64_t>(parameter)));
		}
		else if (std::holds_alternative<double>(parameter))
		{
			pObjectBuilder << pObjectBuilder.setDouble(std::get<double>(parameter));
		}
		else if (std::holds_alternative<float>(parameter))
		{
			pObjectBuilder << pObjectBuilder.setDouble(static_cast<double>(std::get<float>(parameter)));
		}
		else if (std::holds_alternative<const char*>(parameter))
		{
			pObjectBuilder << pObjectBuilder.setString(std::string(std::get<const char*>(parameter)));
		}
		else if (std::holds_alternative<bool>(parameter))
		{
			pObjectBuilder << pObjectBuilder.setBool(std::get<bool>(parameter));
		}
		else
		{
			throw std::exception("Type is not supported by the python service.");
		}
	}
	return pObjectBuilder.getAssembledTuple();
}


std::string PythonAPI::GetModuleEntryPoint(const std::string& moduleName)
{
	PythonObjectBuilder pythonObjectBuilder;
	CPythonObjectBorrowed globalDictionary =_wrapper.GetGlobalDictionary(moduleName);
	CPythonObjectBorrowed allGlobalDictItems = pythonObjectBuilder.getDictItem(globalDictionary);
	
	bool firstOfFunctionNames = false;
	std::string entryPointName = "";
	
	for (int i = 0; i < PyList_Size(allGlobalDictItems); i++)
	{
		CPythonObjectBorrowed listEntry = pythonObjectBuilder.getListItem(allGlobalDictItems,i);
		std::string listEntryName = pythonObjectBuilder.getStringValueFromTuple(listEntry, 0, "ListValue");

		if (listEntryName == "__builtins__") //Last default entry
		{
			firstOfFunctionNames = true;
		}
		else if (firstOfFunctionNames)
		{
			if (entryPointName == "")
			{
				entryPointName = listEntryName;
			}
			else
			{
				entryPointName = "__main__";
				break;
			}
		}
	}
	
	return entryPointName;
}

