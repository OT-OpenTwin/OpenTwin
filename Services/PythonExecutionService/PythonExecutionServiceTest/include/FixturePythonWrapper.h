#pragma once
#include "PythonWrapper.h"
#include <gtest/gtest.h>


class FixturePythonWrapper : public testing::Test
{
public:
	FixturePythonWrapper();
	
	std::string getMainModulName() const { return _mainModuleName; }
	
	void ExecuteString(const std::string& command, const std::string& moduleName);
	std::string ExecuteFunctionWithReturnValue(const std::string& functionName, const std::string& moduleName);
	int ExecuteFunctionWithParameter(const std::string& functionName, int parameter, const std::string& moduleName);
	int ExecuteFunctionWithMultipleParameter(const std::string& functionName, int parameter1, int parameter2, std::string parameter3, const std::string& moduleName);
	int32_t GetGlobalVariable(const std::string& varName, const std::string& moduleName);

private:
	PythonWrapper _wrapper;
	std::string _mainModuleName = "__main__";
};
