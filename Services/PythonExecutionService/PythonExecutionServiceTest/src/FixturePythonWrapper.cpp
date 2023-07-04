#include "FixturePythonWrapper.h"
#include "CPythonObjectNew.h"
#include "PythonObjectBuilder.h"

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
	CPythonObjectNew parameter(nullptr);
	CPythonObjectNew returnVal = _wrapper.ExecuteFunction(functionName, parameter, moduleName);
	return PythonObjectBuilder::INSTANCE().getStringValue(returnVal, "return Value");
}

int64_t FixturePythonWrapper::GetGlobalVariable(const std::string& varName, const std::string& moduleName)
{
	CPythonObjectBorrowed variable = _wrapper.GetGlobalVariable(varName, moduleName);
	return PythonObjectBuilder::INSTANCE().getInt64Value(variable, varName);
}
