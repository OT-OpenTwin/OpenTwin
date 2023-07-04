#include "FixturePythonWrapper.h"
#include <gtest/gtest.h>
TEST_F(FixturePythonWrapper, RepeatedExecution)
{
	const std::string command = "variable = variable + 1";
	const int repetitions = 100000;

	ExecuteString("variable = 0", getMainModulName());
	for (int i = 0; i < repetitions; i++)
	{
		ExecuteString(command, getMainModulName());
	}
	EXPECT_TRUE(true); //Would have thrown an exception in one of the executions
}

TEST_F(FixturePythonWrapper, ThrowWhenPythonSIGABR)
{
	const std::string command = "import os\n os.abort()";
	EXPECT_ANY_THROW(ExecuteString(command, getMainModulName()));
}

TEST_F(FixturePythonWrapper, GetGlobalVariableValue)
{
	ExecuteString("variable = 13",getMainModulName());
	int returnValue = GetGlobalVariable("variable", getMainModulName());
	EXPECT_EQ(returnValue, 13);
}

TEST_F(FixturePythonWrapper, GetGlobalVariableValue_NewModule)
{
	const std::string modulName = "Example";
	ExecuteString("variable = 13", modulName);
	EXPECT_NO_THROW(GetGlobalVariable("variable", modulName));
}
TEST_F(FixturePythonWrapper, GetGlobalVariableValue_WrongModule)
{
	const std::string modulName = "Bubble";
	ExecuteString("variable = 13", modulName);
	EXPECT_ANY_THROW(GetGlobalVariable("variable", getMainModulName()));
}

TEST_F(FixturePythonWrapper, GetGlobalVariableValue_NotExisting)
{
	const std::string modulName = "Example";
	ExecuteString("variable = 13", modulName);
	EXPECT_ANY_THROW(GetGlobalVariable("NotExisting", getMainModulName()));
}

TEST_F(FixturePythonWrapper, FunctionWithReturnValue)
{
	const std::string expectedValue = "Hello World";
	const std::string script = "def example():\n"
								"\tvariable = \"Hello \"\n"
								"\tvariable = variable + \"World\"\n"
								"\treturn variable";
	ExecuteString(script, getMainModulName());
	std::string returnValue = ExecuteFunctionWithReturnValue("example", getMainModulName());
	EXPECT_EQ(expectedValue, returnValue);
}

TEST_F(FixturePythonWrapper, FunctionWithParameter)
{
	const int expectedValue = 26;
	const std::string script = "def example(value):\n"
		"\tvalue = value * 2\n"
		"\treturn value";
	ExecuteString(script, getMainModulName());
	int parameter = 13;
	int returnValue = ExecuteFunctionWithParameter("example",parameter, getMainModulName());
	EXPECT_EQ(expectedValue, returnValue);
}

TEST_F(FixturePythonWrapper, FunctionWithMultipleParameter)
{
	const int expectedValue = 29;
	const std::string script = "def example(value1, value2, stringValue):\n"
		"\tvalue = (value1 + value2) * 2\n"
		"\tif(stringValue == \"Suppe\"):\n"
		"\t\tvalue = value + 3\n"
		"\telse:\n"
		"\t\tvalue = value + 7\n"
		"\treturn value";
	ExecuteString(script, getMainModulName());
	
	int parameter1 = 7;
	int parameter2 = 6;
	std::string parameter3 = "Suppe";

	int returnValue = ExecuteFunctionWithMultipleParameter("example", parameter1, parameter2, parameter3, getMainModulName());
	EXPECT_EQ(expectedValue, returnValue);
}

TEST_F(FixturePythonWrapper, FunctionNotExisting)
{
	const std::string expectedValue = "Hello World";
	const std::string script = "def example():\n"
		"\tvariable = \"Hello \"\n"
		"\tvariable = variable + \"World\"\n"
		"\treturn variable";
	ExecuteString(script, getMainModulName());
	EXPECT_ANY_THROW(ExecuteFunctionWithReturnValue("NotExisting", getMainModulName()));
}

TEST_F(FixturePythonWrapper, PythonExtensionWithParameter)
{
	const int expectedValue = 20;
	const std::string module = "InitialTestModule";
	std::string function = "WithOneParameter";
	int result = ExecuteFunctionWithParameter(function, 7, module);
	EXPECT_EQ(expectedValue, result);
}

TEST_F(FixturePythonWrapper, PythonExtensionWithoutParameter)
{
	const std::string expectedValue = "Hello from the extension";
	const std::string module = "InitialTestModule";
	std::string function = "WithoutParameter";
	std::string result = ExecuteFunctionWithReturnValue(function, module);
	EXPECT_EQ(expectedValue, result);
}