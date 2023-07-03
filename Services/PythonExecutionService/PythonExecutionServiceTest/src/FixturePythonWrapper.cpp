#include "FixturePythonWrapper.h"

FixturePythonWrapper::FixturePythonWrapper()
{
	_wrapper.InitializePythonInterpreter();
}

void FixturePythonWrapper::ExecuteString(const std::string& command, const std::string& moduleName)
{
	_wrapper.Execute(command, moduleName);
}

std::string FixturePythonWrapper::ExecuteFunctionWithReturnValue(const std::string& functionName, const std::string& moduleName)
{
	std::string returnValue;
	_wrapper.ExecuteFunction(functionName, returnValue, moduleName);
	return returnValue;
}

int64_t FixturePythonWrapper::GetGlobalVariable(const std::string& varName, const std::string& moduleName)
{
	int64_t returnVal = 0;
	_wrapper.GetGlobalVariableValue(varName, returnVal,moduleName);
	return returnVal;
}
