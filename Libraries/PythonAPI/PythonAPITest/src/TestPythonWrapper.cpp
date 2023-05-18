#include <gtest/gtest.h>
#include "FixturePythonWrapper.h"
#include "PythonParameter.h"

TEST_F(FixturePythonWrapper, InitializationTestSuccess)
{
	ASSERT_NO_THROW(getPythonWrapper()->InitializePythonInterpreter());
}

TEST_F(FixturePythonWrapper, InitializationTestFailure)
{
	SetFalsePythonSysPath();
	ASSERT_DEATH(getPythonWrapper()->InitializePythonInterpreter(),""); //Problem in initialization causes an os signal (c-level) which cannot be treated with handlers on the level of this application, nor on python level
}

TEST_F(FixturePythonWrapper, SettingGlobalParameterInt)
{
	getPythonWrapper()->setGlobalParameter(GetTestParameterInt32());
	std::string globalParameter = getAllGlobalParameter();
	ASSERT_EQ(globalParameter, GetExpectedParameterStringInt32());
}

TEST_F(FixturePythonWrapper, InitializationWithParameter)
{
	getPythonWrapper()->setGlobalParameter(GetTestParameterInt32());
	ASSERT_NO_THROW(getPythonWrapper()->InitializePythonInterpreter());
}

TEST_F(FixturePythonWrapper, GettingGlobalParameterInt)
{
	getPythonWrapper()->setGlobalParameter(GetTestParameterInt32());
	getPythonWrapper()->InitializePythonInterpreter();
	ASSERT_NO_THROW(getPythonWrapper()->ExecuteString(""));
	std::string globalParameter = getAllGlobalParameter();
	ASSERT_EQ(globalParameter, GetExpectedParameterStringInt32());
}

TEST_F(FixturePythonWrapper, GlobalParameterAlteration)
{
	getPythonWrapper()->setGlobalParameter(GetTestParameterInt32());
	getPythonWrapper()->InitializePythonInterpreter();
	getPythonWrapper()->ExecuteString(GetExecutionStringAlterInt32Var1());
	int32_t newValue = GetInt32Var1();
	int32_t expectedValue = GetExpectedAlteredInt32Var1();
	ASSERT_EQ(newValue, expectedValue );
}