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
		ot::UID uid = baseEntity->getEntityID();
		ot::UID version = baseEntity->getEntityStorageVersion();
		std::string moduleName = std::to_string(uid) + std::to_string(version);
		/*std::string moduleName = baseEntity->getName();
		*/
		_moduleNamesByScriptName[baseEntity->getName()] = moduleName;
		return moduleName;
	};

private:
	PythonLoadedModules() {};
	std::map<std::string, std::string> _moduleNamesByScriptName;

};
