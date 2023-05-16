#pragma once
#include <gtest/gtest.h>
#include "PythonWrapper.h"
#include <string>

class FixturePythonWrapper : public testing::Test
{
public:
	FixturePythonWrapper();
	PythonWrapper* getPythonWrapper();
	std::string getAllGlobalParameter();
	void SetFalsePythonSysPath();
	std::vector< PythonAPI::PythonParameter<int32_t>>* GetTestParameterInt32() { return &_parameter; }
	std::string GetExpectedParameterStringInt32() const {return "var1=2\n"
																"var2=14\n"
																"var3=27\n";
	}
	std::string GetExecutionStringAlterInt32Var1() { return "var1=var1*13"; }
	int32_t GetExpectedAlteredInt32Var1() { return  26; }
	

private:

	PythonWrapper pythonWrapper;

	std::vector< PythonAPI::PythonParameter<int32_t>> _parameter
	{
		 PythonAPI::PythonParameter<int32_t>("var1",2),
		 PythonAPI::PythonParameter<int32_t>("var2",14),
		 PythonAPI::PythonParameter<int32_t>("var3",27)
	};
};


