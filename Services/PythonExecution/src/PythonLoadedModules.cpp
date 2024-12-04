#include "PythonLoadedModules.h"

std::optional<std::string> PythonLoadedModules::getModuleName(const ot::EntityInformation& _scriptEntityInfos)
{
	bool found = false;
	std::string moduleName = createUniqueModuleName(_scriptEntityInfos);
	auto foundModuleName = m_moduleNames.find(moduleName);

	if (foundModuleName == m_moduleNames.end())
	{
		return {};
	}
	else
	{
		return moduleName;
	}
}

std::string PythonLoadedModules::addModuleForEntity(const ot::EntityInformation& _scriptEntityInfo)
{
	const std::string moduleName = createUniqueModuleName(_scriptEntityInfo);
	auto success = m_moduleNames.insert(moduleName);
	if (!success.second)
	{
		OT_LOG_D("Tried to add a module twice. Should not happen.");
	}
	m_entityNameByModuleName[moduleName] = _scriptEntityInfo.getEntityName();
	return moduleName;
}

std::string PythonLoadedModules::createUniqueModuleName(const ot::EntityInformation& _entityInfo)
{
	assert(_entityInfo.getEntityID() != _entityInfo.getEntityVersion());
	assert(_entityInfo.getEntityID() != -1);
	assert(_entityInfo.getEntityID() != 0);
	return std::to_string(_entityInfo.getEntityID()) + std::to_string(_entityInfo.getEntityVersion());
}

std::string PythonLoadedModules::getEntityName(const std::string& _moduleName)
{
	auto entityNameByModuleName = m_entityNameByModuleName.find(_moduleName);
	if (entityNameByModuleName != m_entityNameByModuleName.end())
	{
		return entityNameByModuleName->second;
	}
	else
	{
		return "";
	}
}
