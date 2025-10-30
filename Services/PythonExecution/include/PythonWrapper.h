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
	PythonWrapper(const PythonWrapper& other) = delete;
	PythonWrapper& operator=(const PythonWrapper& other) = delete;
	PythonWrapper(const PythonWrapper&& other) = delete;
	PythonWrapper& operator=(const PythonWrapper&& other) = delete;


	~PythonWrapper();

	static void setSitePackage(const std::string& sitePackageName) { m_customSitePackage = sitePackageName; }
	static void setRedirectOutput(bool redirectOutput) { m_redirectOutput = redirectOutput; }

	void InitializePythonInterpreter();
	void ResetSysPath();
	void AddToSysPath(const std::string& newPathComponent);
	void ClosePythonInterpreter();

	CPythonObjectNew execute(const std::string& _executionCommand, const std::string& _moduleName = "__main__");

	CPythonObjectNew ExecuteFunction(const std::string& functionName, CPythonObject& parameter, const std::string& moduleName = "__main__");

	CPythonObjectBorrowed GetGlobalVariable(const std::string& varName, const std::string& moduleName);

	CPythonObjectBorrowed GetGlobalDictionary(const std::string& moduleName);
	CPythonObjectNew GetFunction(const std::string& functionName, const std::string& moduleName = "__main__");

private:
	std::list<std::string> _pythonPath;
	std::string _pythonRoot;
	std::string _defaultSitePackagesPath;
	bool _interpreterSuccessfullyInitialized = false;
	int pipe_fds[2];

	int initiateNumpy();
	std::string checkNumpyVersion();
	std::string DeterminePythonRootDirectory();
	std::string determineMandatoryPythonSitePackageDirectory();
	void addOptionalUserPythonSitePackageDirectory();
	void readOutput();
	void flushOutput();

	static void signalHandlerAbort(int sig);

	CPythonObjectNew GetModule(const std::string& moduleName);

	static std::string m_customSitePackage;
	static bool m_redirectOutput;
	std::thread* m_outputWorkerThread;
	std::atomic<long long> m_outputProcessingCount = 0;
};
