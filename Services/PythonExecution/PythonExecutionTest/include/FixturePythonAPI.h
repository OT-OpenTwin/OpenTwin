#pragma once
#include <gtest/gtest.h>
#include "CPythonObjectNew.h"
#include <list>
#include "PythonObjectBuilder.h"
#include "PythonAPI.h"
#include "PythonModuleAPI.h"
#include "OpenTwinCore/Variable.h"

class FixturePythonAPI : public testing::Test
{
public:
	CPythonObjectNew CreateParameterSet(std::list<ot::variable_t>& parameterSet) { return _api.CreateParameterSet(parameterSet); };
	std::string GetModuleEntryPoint() { 
		PythonModuleAPI moduleAPI;
		return moduleAPI.GetModuleEntryPoint(_moduleName); 
	};
	
	void SetupModule(const std::string& script);
private:
	PythonWrapper _wrapper;
	PythonAPI _api;
	const std::string _moduleName = "example";
};
