// @otlicense
// File: FixturePythonWrapper.h
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
#include "PythonWrapper.h"
#include <gtest/gtest.h>
#include <list>

class FixturePythonWrapper : public testing::Test
{
public:
	FixturePythonWrapper();
	
	std::string getMainModulName() const { return _mainModuleName; }
	
	void ExecuteString(const std::string& command, const std::string& moduleName);
	std::string ExecuteFunctionWithReturnValue(const std::string& functionName, const std::string& moduleName);
	int ExecuteFunctionWithParameter(const std::string& functionName, int parameter, const std::string& moduleName);
	int ExecuteFunctionWithMultipleParameter(const std::string& functionName, int parameter1, int parameter2, std::string parameter3, const std::string& moduleName);
	int32_t getGlobalVariable(const std::string& varName, const std::string& moduleName);
	std::list<std::string> GetPathVariable(const std::string& moduleName);
	void AddNumpyToPath();
	void ResetPythonPath();

private:
	PythonWrapper _wrapper;
	std::string _mainModuleName = "__main__";
};
