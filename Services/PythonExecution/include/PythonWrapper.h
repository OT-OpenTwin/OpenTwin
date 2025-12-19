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


#include "CPythonObjectBorrowed.h"
#include "CPythonObjectNew.h"
#include "PythonException.h"
#include "InterpreterPathSettings.h"
#include "PackageHandler.h"

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

	//! @brief Initialises the lookup-paths and the entire interpreter. 
	//! If the custom environment name is not empty, it will be added as additional lookup
	void initializePythonInterpreter(InterpreterPathSettings& _interpreterPathSettings);
	void setPackageHandler(PackageHandler* _packageHandler) { m_packageHandler = _packageHandler; }
	void closePythonInterpreter();

	CPythonObjectNew execute(const std::string& _executionCommand, const std::string& _moduleName = "__main__");
	CPythonObjectNew executeFunction(const std::string& _functionName, CPythonObject& _parameter, const std::string& _moduleName = "__main__");
	CPythonObjectBorrowed getGlobalVariable(const std::string& _varName, const std::string& _moduleName);
	CPythonObjectBorrowed getGlobalDictionary(const std::string& _moduleName);
	CPythonObjectNew getFunction(const std::string& _functionName, const std::string& _moduleName = "__main__");

private:
	PackageHandler* m_packageHandler = nullptr;
	bool m_interpreterSuccessfullyInitialized = false;

	static void signalHandlerAbort(int sig);
	void addToSysPath(const std::string& _newPathComponent);

	CPythonObjectNew getModule(const std::string& _moduleName);
};
