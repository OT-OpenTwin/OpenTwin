#pragma once
#include "PythonAPI.h"
#include "PythonWrapper.h"
#include "EntityParameterizedDataSource.h"
#include <list>
#include "PythonObjectBuilder.h"

class PythonAPI
{
	friend class FixturePythonAPI;
public:
	PythonAPI();

	std::list<variable_t> Execute(ot::UIDList& scripts, std::list<std::list<variable_t>>& parameterSet);

private:
	PythonWrapper _wrapper;
	std::map<std::string , std::string> _moduleEntrypointByScriptName;
	std::map<ot::UID, std::string> _scriptNameByUID;

	void EnsureScriptsAreLoaded(ot::UIDList& scripts);
	CPythonObjectNew CreateParameterSet(std::list<variable_t>& parameterSet);
	std::string GetModuleEntryPoint(const std::string& moduleName);
};
