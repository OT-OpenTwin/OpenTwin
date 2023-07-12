#pragma once
#include "PythonAPI.h"
#include "PythonWrapper.h"
#include "EntityParameterizedDataSource.h"
#include <list>
#include "PythonObjectBuilder.h"
#include <optional>

class PythonAPI
{
	friend class FixturePythonAPI;
public:
	PythonAPI();

	std::list<variable_t> Execute(std::list<std::string>& scripts, std::list<std::optional<std::list<variable_t>>>& parameterSet);

private:
	PythonWrapper _wrapper;
	std::map<std::string , std::string> _moduleEntrypointByScriptName;

	void EnsureScriptsAreLoaded(std::list<std::string> scripts);
	CPythonObjectNew CreateParameterSet(std::list<variable_t>& parameterSet);
	std::string GetModuleEntryPoint(const std::string& moduleName);
};
