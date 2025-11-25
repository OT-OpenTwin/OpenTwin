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
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION

#include "PythonObjectBuilder.h"

#include <fcntl.h>
#include <windows.h>
#include <io.h>
#include <iostream>

bool PythonWrapper::m_redirectOutput = false;

PythonWrapper::PythonWrapper() : m_outputWorkerThread(nullptr) 
{
	signal(SIGABRT, &signalHandlerAbort);

	if (m_redirectOutput)
	{
		// Create a pipe for getting the standard output
		if (_pipe(m_pipe_fds, 4096, _O_TEXT) == -1) {
			OT_LOG_EA("Creating pipe for capturing Python output failed.");
		}
	}
}

void PythonWrapper::readOutput() {
	char buffer[256];
	while (m_redirectOutput)
	{
		DWORD bytes_available = 0;
		if (PeekNamedPipe((HANDLE)_get_osfhandle(m_pipe_fds[0]), NULL, 0, NULL, &bytes_available, NULL)) {
			if (bytes_available > 0) {
				int count = _read(m_pipe_fds[0], buffer, sizeof(buffer) - 1);
				if (count > 0) {
					buffer[count] = '\0';
					//Application::instance().getCommunicationHandler().writeToServer("OUTPUT:" + std::to_string(strlen(buffer)) + ":" + std::string(buffer));
					Application::instance().getCommunicationHandler().writeToServer("OUTPUT:" + std::string(buffer));
				}
			}
		}

		m_outputProcessingCount++;

		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
}

int PythonWrapper::initiateNumpy() {
	
	return 1;
}


PythonWrapper::~PythonWrapper() {
	if (!m_interpreterSuccessfullyInitialized) {
		closePythonInterpreter();
	}

	if (m_outputWorkerThread)
	{
		m_redirectOutput = false;
		m_outputWorkerThread->join();

		delete m_outputWorkerThread;
		m_outputWorkerThread = nullptr;
	}
}

void PythonWrapper::closePythonInterpreter() {
	int success = Py_FinalizeEx();
	if (success == -1) {
		throw PythonException();
	}
}

#include "OTCore/String.h"

void PythonWrapper::initializePythonInterpreter(const std::string& _environmentName)
{
	std::list<std::wstring> lookupPaths;
	std::string devEnvRootName = "OPENTWIN_DEV_ROOT";
	const char* devEnvRoot = ot::OperatingSystem::getEnvironmentVariable(devEnvRootName.c_str());
	std::wstring devEnvRootW = ot::String::toWString(devEnvRoot);
#ifdef _DEBUG
	const std::string pythonRootEnvVarName = "OT_PYTHON_ROOT";
	const char* pythonRoot = ot::OperatingSystem::getEnvironmentVariable(pythonRootEnvVarName.c_str());
	assert(pythonRoot != nullptr);
	std::wstring pythonRootW = ot::String::toWString(pythonRoot);
	//Here we have the standart libs
	lookupPaths.push_back(pythonRootW + std::wstring(L"\\Environments\\CoreEnvironment\\Debug\\DLLs"));
	lookupPaths.push_back(pythonRootW + std::wstring(L"\\Environments\\CoreEnvironment\\Debug\\Lib"));

	//Here we have the third party packages
	lookupPaths.push_back(devEnvRootW + std::wstring(L"\\Deployment\\Environments\\") + ot::String::toWString(_environmentName));
	//Here we have the dlls
	lookupPaths.push_back(pythonRootW + std::wstring(L"\\Interpreter\\Debug"));
#else
	// If not found, we are in deployment mode
	std::wstring pathBase;
	if (devEnvRoot == nullptr)
	{
		pathBase = L".\\Python";
	}
	else
	{
		pathBase = devEnvRootW + L"\\Deployment\\Python";
	}

	//Here we have the standart libs
	lookupPaths.push_back(pathBase + std::wstring(L"\\Environments\\CoreEnvironment\\DLLs"));
	lookupPaths.push_back(pathBase + std::wstring(L"\\Environments\\CoreEnvironment\\Lib"));

	//Here we have the third party packages
	lookupPaths.push_back(pathBase + std::wstring(L"\\Environments\\") + ot::String::toWString(_environmentName)); 
	//Here we have the dlls
	lookupPaths.push_back(pathBase + std::wstring(L"\\Interpreter"));
#endif

	PyConfig config;
	PyConfig_InitPythonConfig(&config);

	std::string debugPathOverview = "";
	for(std::wstring& pathComponent : lookupPaths) {
		PyWideStringList_Append(&config.module_search_paths, pathComponent.c_str());
		debugPathOverview += ot::String::toString(pathComponent) + ";";
	}
	OT_LOG_D("Python path: " + debugPathOverview);
	
	//config.executable = L"CoreEnvironment/Debug/python_d.exe";
	//config.prefix = L"CoreEnvironment/Debug";
	//config.home = L"CoreEnvironment/Debug";

		
	int errorCode = PyImport_AppendInittab("OpenTwin", PythonExtensions::PyInit_OpenTwin);
	
	Py_DontWriteBytecodeFlag = 1;
	Py_QuietFlag = 1;
	Py_InitializeFromConfig(&config);
	if (Py_IsInitialized() != 1) {
		throw PythonException();
	}
	m_interpreterSuccessfullyInitialized = true;
	
	if (m_redirectOutput)
	{
		// Redirect output to pipe
		std::string command =
			"import sys\n"
			"import os\n"
			"import logging\n"
			"sys.stdout = os.fdopen(" + std::to_string(m_pipe_fds[1]) + ", 'w')\n"
			"sys.stdout.reconfigure(line_buffering = True)\n"
			"logging.getLogger().addHandler(logging.StreamHandler(sys.stdout))\n";
			
		PyRun_SimpleString(command.c_str());

		m_outputWorkerThread = new std::thread(&PythonWrapper::readOutput, this);
	}
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

void PythonWrapper::flushOutput()
{
	if (m_redirectOutput && m_outputWorkerThread != nullptr)
	{
		long long currentOutputProcessingCount = m_outputProcessingCount;

		// Wait until all messages have been sent
		while (m_outputProcessingCount < currentOutputProcessingCount + 2)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
}

CPythonObjectNew PythonWrapper::execute(const std::string& _executionCommand, const std::string& _moduleName) {
	CPythonObjectNew module(getModule(_moduleName));
	CPythonObjectBorrowed globalDirectory(PyModule_GetDict(module));
	CPythonObjectNew result(PyRun_String(_executionCommand.c_str(), Py_file_input, globalDirectory, globalDirectory));

	flushOutput();

	if (result == nullptr)
	{
		throw PythonException();
	}

	return result;
}

CPythonObjectNew PythonWrapper::executeFunction(const std::string& _functionName, CPythonObject& _parameter, const std::string& _moduleName) {
	CPythonObjectNew function(getFunction(_functionName, _moduleName)); //Really borrowed?
	CPythonObjectNew returnValue(PyObject_CallObject(function, _parameter));
	
	flushOutput();

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
