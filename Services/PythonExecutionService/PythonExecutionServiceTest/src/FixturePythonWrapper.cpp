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
	return PythonObjectBuilder::INSTANCE()->getStringValue(returnVal, "return Value");
}

int FixturePythonWrapper::ExecuteFunctionWithParameter(const std::string& functionName, int parameter, const std::string& moduleName)
{
	PythonObjectBuilder::INSTANCE()->StartTupleAssemply(1);
	*PythonObjectBuilder::INSTANCE() << PythonObjectBuilder::INSTANCE()->setInt32(parameter);
	CPythonObjectNew pythonParameter = PythonObjectBuilder::INSTANCE()->getAssembledTuple();

	CPythonObjectNew returnVal = _wrapper.ExecuteFunction(functionName, pythonParameter, moduleName);
	return PythonObjectBuilder::INSTANCE()->getInt32Value(returnVal, "return Value");
}

int FixturePythonWrapper::ExecuteFunctionWithMultipleParameter(const std::string& functionName, int parameter1, int parameter2, std::string parameter3, const std::string& moduleName)
{
	PythonObjectBuilder::INSTANCE()->StartTupleAssemply(3);
	*PythonObjectBuilder::INSTANCE() << PythonObjectBuilder::INSTANCE()->setInt32(parameter1);
	*PythonObjectBuilder::INSTANCE() << PythonObjectBuilder::INSTANCE()->setInt32(parameter2);
	*PythonObjectBuilder::INSTANCE() << PythonObjectBuilder::INSTANCE()->setString(parameter3);
	CPythonObjectNew pythonParameter = PythonObjectBuilder::INSTANCE()->getAssembledTuple();

	CPythonObjectNew returnVal = _wrapper.ExecuteFunction(functionName, pythonParameter, moduleName);
	return PythonObjectBuilder::INSTANCE()->getInt32Value(returnVal, "return Value");
}


int32_t FixturePythonWrapper::GetGlobalVariable(const std::string& varName, const std::string& moduleName)
{
	CPythonObjectBorrowed variable = _wrapper.GetGlobalVariable(varName, moduleName);
	return PythonObjectBuilder::INSTANCE()->getInt32Value(variable, varName);
}
