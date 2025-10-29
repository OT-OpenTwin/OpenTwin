// @otlicense

#pragma once
#include <optional>
#include <list>
#include "PythonWrapper.h"
#include "EntityFile.h"
#include "PythonObjectBuilder.h"
#include "EntityInformation.h"
#include "OTCore/ReturnValues.h"

class PythonAPI
{
	friend class FixturePythonAPI;
public:
	PythonAPI();
	void execute(std::list<std::string>& _scripts, std::list<std::list<ot::Variable>>& _parameterSet) noexcept(false);
	void execute(const std::string& _command) noexcept(false);
private:
	std::map<std::string , std::string> m_moduleEntrypointByModuleName;
	PythonWrapper m_wrapper;
	std::list<ot::EntityInformation> ensureScriptsAreLoaded(const std::list<std::string>& _scripts);
	void loadScipt(const ot::EntityInformation& _entityInformation);
};
