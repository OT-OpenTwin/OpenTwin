// @otlicense
// File: PythonWrapper.h
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

#pragma once
#define PY_SSIZE_T_CLEAN // # 
#include <Python.h>
#include <string>
#include <vector>
#include <list>
#include <thread>
#include <atomic>

#include "CPythonObjectBorrowed.h"
#include "CPythonObjectNew.h"
#include "PythonException.h"

//! @brief Wrapper around python code execution.
class PythonWrapper {
	friend class FixturePythonWrapper;
public:
	PythonWrapper();
	PythonWrapper(const PythonWrapper& _other) = delete;
	PythonWrapper& operator=(const PythonWrapper& _other) = delete;
	PythonWrapper(const PythonWrapper&& _other) = delete;
	PythonWrapper& operator=(const PythonWrapper&& _other) = delete;

	~PythonWrapper();

	static void setSitePackage(const std::string& _sitePackageName) { m_customSitePackage = _sitePackageName; }
	static void setRedirectOutput(bool _redirectOutput) { m_redirectOutput = _redirectOutput; }

	void initializePythonInterpreter();
	void resetSysPath();
	void addToSysPath(const std::string& _newPathComponent);
	void closePythonInterpreter();

	CPythonObjectNew execute(const std::string& _executionCommand, const std::string& _moduleName = "__main__");
	CPythonObjectNew executeFunction(const std::string& _functionName, CPythonObject& _parameter, const std::string& _moduleName = "__main__");
	CPythonObjectBorrowed getGlobalVariable(const std::string& _varName, const std::string& _moduleName);
	CPythonObjectBorrowed getGlobalDictionary(const std::string& _moduleName);
	CPythonObjectNew getFunction(const std::string& _functionName, const std::string& _moduleName = "__main__");

private:
	static std::string m_customSitePackage;
	static bool m_redirectOutput;

	std::list<std::string> m_pythonPath;
	std::string m_pythonRoot;
	std::string m_defaultSitePackagesPath;
	bool m_interpreterSuccessfullyInitialized = false;
	int m_pipe_fds[2];
	std::thread* m_outputWorkerThread;
	std::atomic<long long> m_outputProcessingCount = 0;

	static void signalHandlerAbort(int sig);

	int initiateNumpy();
	std::string checkNumpyVersion();
	std::string determinePythonRootDirectory();
	std::string determineMandatoryPythonSitePackageDirectory();
	void addOptionalUserPythonSitePackageDirectory();
	void readOutput();
	void flushOutput();
	CPythonObjectNew getModule(const std::string& _moduleName);
};
