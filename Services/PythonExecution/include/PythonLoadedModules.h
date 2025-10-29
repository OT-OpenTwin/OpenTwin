// @otlicense

/*****************************************************************//**
 * \file   PythonLoadedModules.h
 * \brief  Meyer's singleton that holds a map of all modules that were loaded in the python interpreter, sorted by the name of the corresponding script name.
 * 
 * \author Wagner
 * \date   July 2023
 *********************************************************************/
#pragma once
#include <string>
#include <set>
#include <map>
#include <optional>

#include "EntityInformation.h"
#include "OTCore/LogDispatcher.h"

class PythonLoadedModules
{
public:
	static PythonLoadedModules& instance(void);
	
	std::optional<std::string> getModuleName(const ot::EntityInformation& _scriptEntityInfos);
	std::string addModuleForEntity(const ot::EntityInformation& _scriptEntityInfo);
	std::string createUniqueModuleName(const ot::EntityInformation& _entityInfo);
	
	std::string getEntityName(const std::string& _moduleName);
private:

	PythonLoadedModules() {};
	std::set<std::string> m_moduleNames;
	std::map<std::string, std::string> m_entityNameByModuleName;
};
