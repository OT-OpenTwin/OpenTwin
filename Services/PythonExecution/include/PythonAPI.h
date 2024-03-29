#pragma once
#include <optional>
#include <list>
#include "PythonWrapper.h"
#include "EntityFile.h"
#include "PythonObjectBuilder.h"
#include "OTServiceFoundation/EntityInformation.h"
#include "OTCore/ReturnValues.h"

class PythonAPI
{
	friend class FixturePythonAPI;
public:
	PythonAPI();
	ot::ReturnValues Execute(std::list<std::string>& scripts, std::list<std::list<ot::Variable>>& parameterSet) noexcept(false);
	ot::ReturnValues Execute(const std::string& command) noexcept(false);
private:
	std::map<std::string , std::string> _moduleEntrypointByModuleName;
	PythonWrapper _wrapper;
	std::list<ot::EntityInformation> EnsureScriptsAreLoaded(std::list<std::string> scripts);
	void LoadScipt(ot::EntityInformation& entityInformation);
};
