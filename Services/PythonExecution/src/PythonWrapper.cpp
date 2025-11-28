// @otlicense
// File: PythonWrapper.cpp
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

#include <signal.h>
#include "PythonWrapper.h"
#include "PythonExtension.h"
#include "Application.h"

#include "OTSystem/OperatingSystem.h"
#include "OTCore/LogDispatcher.h"

#define PY_ARRAY_UNIQUE_SYMBOL PythonWrapper_ARRAY_API

#include "PythonObjectBuilder.h"
#include "OTCore/String.h"
#include <iostream>
#include "OutputPipeline.h"

PythonWrapper::PythonWrapper()
{
	signal(SIGABRT, &signalHandlerAbort);
	OutputPipeline::instance().setupOutputPipeline();
}

PythonWrapper::~PythonWrapper() {
	if (!m_interpreterSuccessfullyInitialized) {
		closePythonInterpreter();
	}


}

void PythonWrapper::closePythonInterpreter() {
	int success = Py_FinalizeEx();
	if (success == -1) {
		throw PythonException();
	}
}


void PythonWrapper::initializePythonInterpreter(const std::string& _environmentName)
{
	std::string devEnvRootName = "OPENTWIN_DEV_ROOT";
	const char* devEnvRoot = ot::OperatingSystem::getEnvironmentVariable(devEnvRootName.c_str());
	std::wstring devEnvRootW = ot::String::toWString(devEnvRoot);

	std::wstring environmentsBase, home, dllPath, binPath;
#ifdef _DEBUG
	const std::string pythonRootEnvVarName = "OT_PYTHON_ROOT";
	const char* pythonRoot = ot::OperatingSystem::getEnvironmentVariable(pythonRootEnvVarName.c_str());
	assert(pythonRoot != nullptr);
	std::wstring pythonRootW = ot::String::toWString(pythonRoot);
	
	environmentsBase = pythonRootW + L"\\Environments";
	home = environmentsBase + std::wstring(L"\\CoreEnvironment");
	dllPath = +L"\\DLLs\\Debug";
	binPath = pythonRootW + L"\\Interpreter\\Debug";

#elif _RELEASEDEBUG
	const std::string pythonRootEnvVarName = "OT_PYTHON_ROOT";
	const char* pythonRoot = ot::OperatingSystem::getEnvironmentVariable(pythonRootEnvVarName.c_str());
	assert(pythonRoot != nullptr);
		
	std::wstring pythonRootW = ot::String::toWString(pythonRoot);
	environmentsBase = pythonRootW + L"\\Environments";
	home = environmentsBase + std::wstring(L"\\CoreEnvironment");
	dllPath = + L"\\DLLs\\Release";
	binPath = pythonRootW + L"\\Interpreter\\Release";
#else
	// If not found, we are in deployment mode
	if (devEnvRoot == nullptr)
	{
		environmentsBase = L".\\PythonEnvironments";
		binPath = L".";
	}
	else
	{
		environmentsBase = devEnvRootW + L"\\Deployment\\PythonEnvironments";
		binPath = devEnvRootW + L"\\Deployment";
	}
	dllPath = L"\\DLLs";

	//Here we have the standart libs
	home = environmentsBase + std::wstring(L"\\CoreEnvironment");

#endif

	std::list<std::wstring> lookupPaths;

	lookupPaths.push_back(environmentsBase + std::wstring(L"\\PythonBuildTools"));
	lookupPaths.push_back(environmentsBase + std::wstring(L"\\PyritEnvironment"));
	lookupPaths.push_back(home);
	lookupPaths.push_back(home + L"\\Lib");
	lookupPaths.push_back(home+ dllPath);
	lookupPaths.push_back(binPath);

	if (!_environmentName.empty())
	{
		std::wstring environmentPathW = environmentsBase + L"\\" + ot::String::toWString(_environmentName);
		lookupPaths.push_back(environmentPathW);
		m_environmentPath = ot::String::toString(environmentPathW);
		OT_LOG_D("Custom environment: " + m_environmentPath);
	}
	else
	{
		OT_LOG_D("Running without custom environment");
	}

	PyConfig config;
	PyConfig_InitPythonConfig(&config);
	config.module_search_paths_set = 1;

	std::string debugPathOverview = "";
	for(std::wstring& pathComponent : lookupPaths) {
		PyWideStringList_Append(&config.module_search_paths, pathComponent.c_str());
		debugPathOverview += ot::String::toString(pathComponent) + ";";
	}

	PyConfig_SetString(&config,&config.home, home.c_str());

	OT_LOG_D("Python path: " + debugPathOverview);
	OT_LOG_D("Python home: " + ot::String::toString(home));
		
	int errorCode = PyImport_AppendInittab("OpenTwin", PythonExtensions::PyInit_OpenTwin);
	
	Py_DontWriteBytecodeFlag = 1;
	Py_QuietFlag = 1;
	Py_InitializeFromConfig(&config);
	if (Py_IsInitialized() != 1) {
		throw PythonException();
	}
	m_interpreterSuccessfullyInitialized = true;
	
	OutputPipeline::instance().initiateRedirect();
}


void PythonWrapper::addToSysPath(const std::string& _newPathComponent) {
	const std::string addToPythonPath = "import sys\n"
		"sys.path.append(\""+_newPathComponent+"\")\n";
	execute(addToPythonPath);
}

void PythonWrapper::signalHandlerAbort(int _sig) {
	signal(SIGABRT, &signalHandlerAbort);
	throw std::exception("Abort was called.");
}



CPythonObjectNew PythonWrapper::execute(const std::string& _executionCommand, const std::string& _moduleName) {
	CPythonObjectNew module(getModule(_moduleName));
	CPythonObjectBorrowed globalDirectory(PyModule_GetDict(module));
	CPythonObjectNew result(PyRun_String(_executionCommand.c_str(), Py_file_input, globalDirectory, globalDirectory));

	OutputPipeline::instance().flushOutput();

	if (result == nullptr)
	{
		throw PythonException();
	}

	return result;
}

CPythonObjectNew PythonWrapper::executeFunction(const std::string& _functionName, CPythonObject& _parameter, const std::string& _moduleName) {
	CPythonObjectNew function(getFunction(_functionName, _moduleName)); //Really borrowed?
	CPythonObjectNew returnValue(PyObject_CallObject(function, _parameter));
	
	OutputPipeline::instance().flushOutput();

	if (!returnValue.ReferenceIsSet())
	{
		throw PythonException();
	}
	return returnValue;
}

CPythonObjectBorrowed PythonWrapper::getGlobalVariable(const std::string& _varName, const std::string& _moduleName) {
	CPythonObjectNew module = (getModule(_moduleName));
	CPythonObjectBorrowed globalDirectory(PyModule_GetDict(module));
	CPythonObjectBorrowed pythonVar(PyDict_GetItemString(globalDirectory, _varName.c_str()));
	if (pythonVar == nullptr)
	{
		throw PythonException();
	}
	return pythonVar;
}

CPythonObjectBorrowed PythonWrapper::getGlobalDictionary(const std::string& _moduleName) {
	CPythonObjectBorrowed module(PyImport_AddModule(_moduleName.c_str()));
	return PyModule_GetDict(module);
}

CPythonObjectNew PythonWrapper::getFunction(const std::string& _functionName, const std::string& _moduleName) {

	CPythonObjectNew module = getModule(_moduleName);
	CPythonObjectNew function(PyObject_GetAttrString(module, _functionName.c_str()));
	if (function == nullptr)
	{
		throw PythonException();
	}

	if (PyCallable_Check(function) != 1)
	{
		throw std::exception(("Function " + _functionName + " does not exist in module " + _moduleName).c_str());
	}
	return function;
}

CPythonObjectNew PythonWrapper::getModule(const std::string& _moduleName) {
	PythonObjectBuilder builder;
	PyObject* module(PyImport_ImportModule(_moduleName.c_str()));

	if (module == nullptr)
	{
		PyObject* type, * value, * traceback;
		PyErr_Fetch(&type, &value, &traceback);
		if (_moduleName == "__main__")
		{
			throw PythonException();
		}
		CPythonObjectBorrowed newModule(PyImport_AddModule(_moduleName.c_str()));
		return PyImport_ImportModule(_moduleName.c_str());
	}
	else
	{
		return module;	
	}
}
