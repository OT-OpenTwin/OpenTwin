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

std::string PythonWrapper::m_customSitePackage;
bool PythonWrapper::m_redirectOutput = false;

PythonWrapper::PythonWrapper() : m_outputWorkerThread(nullptr) {
	m_pythonRoot = determinePythonRootDirectory();
	
	OT_LOG_D("Setting Python root path: " + m_pythonRoot);
	m_pythonPath.push_back(m_pythonRoot);
	m_pythonPath.push_back(m_pythonRoot + "\\Lib");
	m_pythonPath.push_back(m_pythonRoot + "\\DLLs");
	m_sitePackagesPath = determineMandatoryPythonSitePackageDirectory();
	m_pythonPath.push_back(m_sitePackagesPath);

	if (m_customSitePackage.empty())
	{
		addOptionalUserPythonSitePackageDirectory();
	}

	OT_LOG_D("Setting Python site-package path: " + m_sitePackagesPath);
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

std::string PythonWrapper::checkNumpyVersion() {
	execute("import numpy\n"
		"numpyVersion = numpy.version.version", "__main__");
	CPythonObjectBorrowed variable = getGlobalVariable("numpyVersion","__main__");
	PythonObjectBuilder builder;
	const std::string numpyVersion = builder.getStringValue(variable, "numpyVersion");
	return numpyVersion;
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

void PythonWrapper::initializePythonInterpreter() {
	std::wstring allPaths;
	std::string debugPathOverview("");
	for (std::string& pathComponent : m_pythonPath) {
		std::wstring temp(pathComponent.begin(), pathComponent.end());
		allPaths += temp + L";";
		debugPathOverview += pathComponent;
	}

	OT_LOG_D("Python path: " + debugPathOverview);
	
	int errorCode = PyImport_AppendInittab("OpenTwin", PythonExtensions::PyInit_OpenTwin);
	Py_SetPath(allPaths.c_str());
	int skipSignalHandlerRegistration = 0; //0: Skips; 1: not skipping (equivalent to Py_Initialize())
	Py_DontWriteBytecodeFlag = 1;
	Py_QuietFlag = 1;
	Py_InitializeEx(skipSignalHandlerRegistration);
	if (Py_IsInitialized() != 1) {
		throw PythonException();
	}
	m_interpreterSuccessfullyInitialized = true;
	int numpyErrorCode = initiateNumpy();
	if (numpyErrorCode == 0) {
		throw PythonException("Numpy Initialization failed. Make sure that the file numpy/ndarrayobject.h exists. Details: ");
	}

	const std::string numpyVersion = checkNumpyVersion();
	OT_LOG_D("Initiated numpy version: " + numpyVersion);

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

void PythonWrapper::resetSysPath() {
	std::string resetPythonPath = "import sys\n"
		"sys.path.clear()\n";
	
	for (std::string& pathComponent : m_pythonPath) {
		resetPythonPath += "sys.path.append(\"" +pathComponent +"\")\n";
	}
	execute(resetPythonPath);
}

void PythonWrapper::addToSysPath(const std::string& _newPathComponent) {
	const std::string addToPythonPath = "import sys\n"
		"sys.path.append(\""+_newPathComponent+"\")\n";
	execute(addToPythonPath);
}

std::string PythonWrapper::determinePythonRootDirectory() {
	std::string path;

	std::string devEnvRootName = "OPENTWIN_DEV_ROOT";
	const char* devEnvRoot = ot::OperatingSystem::getEnvironmentVariable(devEnvRootName.c_str());
	if (devEnvRoot == nullptr)
	{
		path = ".\\Python";
	}
	else
	{
		path = std::string(devEnvRoot) + "\\Deployment\\Python";
	}
	return path;
}

std::string PythonWrapper::determineMandatoryPythonSitePackageDirectory() {

	std::string path = determinePythonRootDirectory();

	if (!m_customSitePackage.empty())
	{
		path += "\\Lib\\" + m_customSitePackage;

	}
	else
	{
		path += "\\Lib\\site-packages";
	}
	return path;
}

void PythonWrapper::addOptionalUserPythonSitePackageDirectory()
{
	std::string envName = "OT_PYTHON_SITE_PACKAGE_PATH";
	const char* pythonSitePackagePath = ot::OperatingSystem::getEnvironmentVariable(envName.c_str());

	if (pythonSitePackagePath != nullptr)
	{
		m_pythonPath.push_back(pythonSitePackagePath);
	}
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
