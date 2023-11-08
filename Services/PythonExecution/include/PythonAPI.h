#pragma once
#include <optional>
#include <list>
#include "PythonAPI.h"
#include "PythonWrapper.h"
#include "EntityFile.h"
#include "PythonObjectBuilder.h"

class PythonAPI
{
	friend class FixturePythonAPI;
public:
	PythonAPI();

	std::list<ot::Variable> Execute(std::list<std::string>& scripts, std::list<std::optional<std::list<ot::Variable>>>& parameterSet) noexcept(false);

private:
	std::map<std::string , std::string> _moduleEntrypointByScriptName;
	PythonWrapper _wrapper;
	void EnsureScriptsAreLoaded(std::list<std::string> scripts);
};
