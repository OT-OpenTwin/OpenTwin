#pragma once
#include <gtest/gtest.h>
#include "CPythonObjectNew.h"
#include <list>
#include "PythonObjectBuilder.h"
#include "PythonAPI.h"

class FixturePythonAPI : public testing::Test
{
public:
	CPythonObjectNew CreateParameterSet(std::list<variable_t>& parameterSet) { return _api.CreateParameterSet(parameterSet); };
	std::string GetModuleEntryPoint() { return _api.GetModuleEntryPoint(_moduleName); };
	
	void SetupModule(const std::string& script);
private:
	PythonAPI _api;
	const std::string _moduleName = "example";
};
