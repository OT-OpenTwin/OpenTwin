#include "FixturePythonWrapper.h"
#include "CPythonObjectNew.h"
#include "PythonObjectBuilder.h"
#include "TestingPythonExtensions.h"

FixturePythonWrapper::FixturePythonWrapper()
{
	int errorCode = PyImport_AppendInittab("InitialTestModule", TestingPythonExtensions::PyInit_Testing);
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
	PythonObjectBuilder pyObBuilder;
	return pyObBuilder.getStringValue(returnVal, "return Value");
}

int FixturePythonWrapper::ExecuteFunctionWithParameter(const std::string& functionName, int parameter, const std::string& moduleName)
{
	PythonObjectBuilder pyObBuilder;
	pyObBuilder.StartTupleAssemply(1);
	auto value = pyObBuilder.setInt32(parameter);
	pyObBuilder << &value;
	CPythonObjectNew pythonParameter = pyObBuilder.getAssembledTuple();

	CPythonObjectNew returnVal = _wrapper.ExecuteFunction(functionName, pythonParameter, moduleName);
	return pyObBuilder.getInt32Value(returnVal, "return Value");
}

int FixturePythonWrapper::ExecuteFunctionWithMultipleParameter(const std::string& functionName, int parameter1, int parameter2, std::string parameter3, const std::string& moduleName)
{
	PythonObjectBuilder pyObBuilder;
	pyObBuilder.StartTupleAssemply(3);
	auto param1 = pyObBuilder.setInt32(parameter1);
	pyObBuilder << &param1;
	auto param2 = pyObBuilder.setInt32(parameter2);
	pyObBuilder << &param2;
	auto param3 = pyObBuilder.setString(parameter3);
	pyObBuilder << &param3;

	CPythonObjectNew pythonParameter = pyObBuilder.getAssembledTuple();
	
	CPythonObjectNew returnVal = _wrapper.ExecuteFunction(functionName, pythonParameter, moduleName);
	return pyObBuilder.getInt32Value(returnVal, "return Value");
}


int32_t FixturePythonWrapper::GetGlobalVariable(const std::string& varName, const std::string& moduleName)
{
	PythonObjectBuilder pyObBuilder;
	CPythonObjectBorrowed variable = _wrapper.GetGlobalVariable(varName, moduleName);
	return pyObBuilder.getInt32Value(variable, varName);
}

std::list<std::string> FixturePythonWrapper::GetPathVariable(const std::string& moduleName)
{
	PythonObjectBuilder pyObBuilder;
	CPythonObjectBorrowed variable = _wrapper.GetGlobalVariable("path", moduleName);
	return pyObBuilder.getStringList(variable, "path");
}

void FixturePythonWrapper::AddNumpyToPath()
{
	_wrapper.AddToSysPath("C:\\ThirdParty\\Python\\Python310\\Lib\\site-packages");
}

void FixturePythonWrapper::ResetPythonPath()
{
	_wrapper.ResetSysPath();
}
