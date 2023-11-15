/*****************************************************************//**
 * \file   PythonLoadedModules.h
 * \brief  Meyer's singleton that holds a map of all modules that were loaded in the python interpreter, sorted by the name of the corresponding script name.
 * 
 * \author Wagner
 * \date   July 2023
 *********************************************************************/
#pragma once
#include <map>
#include <string>
#include <optional>

class PythonLoadedModules
{
public:
	static PythonLoadedModules* INSTANCE()
	{
		static PythonLoadedModules instance;
		return &instance;
	}
	
	std::optional<std::string> getModuleName(const std::string& scriptName) 
	{
		if (_moduleNamesByScriptName.find(scriptName) == _moduleNamesByScriptName.end())
		{
			return {};
		}
		else
		{
			return _moduleNamesByScriptName[scriptName]; 
		}
	}
	std::string AddModuleForEntity(EntityBase* baseEntity)
	{
		std::string moduleName = CreateModuleName(baseEntity->getEntityID(), baseEntity->getEntityStorageVersion());
		_moduleNamesByScriptName[baseEntity->getName()] = moduleName;
		return moduleName;
	};
	std::string GetModuleName(ot::EntityInformation& entityInfo)
	{
		return CreateModuleName(entityInfo.getID(), entityInfo.getVersion());
	}
	void RemoveModule(const std::string& scriptName)
	{
		_moduleNamesByScriptName.erase(scriptName);
	}

private:
	std::string CreateModuleName(ot::UID entityID, ot::UID entityVersion)
	{
		return std::to_string(entityID) + std::to_string(entityVersion);
	}
	
	PythonLoadedModules() {};
	std::map<std::string, std::string> _moduleNamesByScriptName;

};
