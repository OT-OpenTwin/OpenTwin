// @otlicense
// File: FixturePythonWrapper.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#include "FixturePythonWrapper.h"
#include "CPythonObjectNew.h"
#include "PythonObjectBuilder.h"
#include "TestingPythonExtensions.h"

FixturePythonWrapper::FixturePythonWrapper()
{
	int errorCode = PyImport_AppendInittab("InitialTestModule", TestingPythonExtensions::PyInit_Testing);
	_wrapper.initializePythonInterpreter();
}

void FixturePythonWrapper::ExecuteString(const std::string& command, const std::string& moduleName)
{
	_wrapper.execute(command, moduleName);
}

std::string FixturePythonWrapper::ExecuteFunctionWithReturnValue(const std::string& functionName, const std::string& moduleName)
{
	CPythonObjectNew parameter(nullptr);
	CPythonObjectNew returnVal = _wrapper.executeFunction(functionName, parameter, moduleName);
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

	CPythonObjectNew returnVal = _wrapper.executeFunction(functionName, pythonParameter, moduleName);
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
	
	CPythonObjectNew returnVal = _wrapper.executeFunction(functionName, pythonParameter, moduleName);
	return pyObBuilder.getInt32Value(returnVal, "return Value");
}


int32_t FixturePythonWrapper::getGlobalVariable(const std::string& varName, const std::string& moduleName)
{
	PythonObjectBuilder pyObBuilder;
	CPythonObjectBorrowed variable = _wrapper.getGlobalVariable(varName, moduleName);
	return pyObBuilder.getInt32Value(variable, varName);
}

std::list<std::string> FixturePythonWrapper::GetPathVariable(const std::string& moduleName)
{
	PythonObjectBuilder pyObBuilder;
	CPythonObjectBorrowed variable = _wrapper.getGlobalVariable("path", moduleName);
	return pyObBuilder.getStringList(variable, "path");
}

void FixturePythonWrapper::AddNumpyToPath()
{
	_wrapper.addToSysPath("C:\\ThirdParty\\Python\\Python310\\Lib\\site-packages");
}

void FixturePythonWrapper::ResetPythonPath()
{
	_wrapper.resetSysPath();
}
