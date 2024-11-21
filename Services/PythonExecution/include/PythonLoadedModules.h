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
#include <set>
#include <optional>

class PythonLoadedModules
{
public:
	static PythonLoadedModules* INSTANCE()
	{
		static PythonLoadedModules instance;
		return &instance;
	}
	
	std::optional<std::string> getModuleName(ot::EntityInformation& scriptEntityInfos) 
	{
		bool found = false;
		std::string moduleName = CreateUniqueModuleName(scriptEntityInfos);
		for (std::string& name : _moduleNames)
		{
			if (moduleName == name)
			{
				found = true;
				break;
			}
		}
		
		if(!found)
		{
			return {};
		}
		else
		{
			return moduleName;
		}
	}

	std::string AddModuleForEntity(ot::EntityInformation& scriptEntityInfo)
	{
		std::string moduleName = CreateUniqueModuleName(scriptEntityInfo);
		_moduleNames.push_back(moduleName);
		return moduleName;
	};

	std::string CreateUniqueModuleName(ot::EntityInformation& entityInfo)
	{
		return std::to_string(entityInfo.getEntityID()) + std::to_string(entityInfo.getEntityVersion());
	}
private:

	PythonLoadedModules() {};
	std::list<std::string> _moduleNames;

};
