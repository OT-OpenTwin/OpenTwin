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
	int64_t GetGlobalVariable(const std::string& varName, const std::string& moduleName);
	void StartExecutionSequence(const std::string& moduleName);
	void EndExecutionSequence();

private:
	PythonWrapper _wrapper;
	std::string _mainModuleName = "__main__";

};
