// @otlicense
// File: FixturePythonAPI.h
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
#include <gtest/gtest.h>
#include "CPythonObjectNew.h"
#include <list>
#include "PythonObjectBuilder.h"
#include "PythonAPI.h"
#include "PythonModuleAPI.h"
#include "OTCore/Variable.h"
#include "OTCore/ReturnMessage.h"

class FixturePythonAPI : public testing::Test
{
public:
	FixturePythonAPI()
	{
		EntityBase::setUidGenerator(new DataStorageAPI::UniqueUIDGenerator(123, 456));
	}
	CPythonObjectNew CreateParameterSet(std::list<ot::Variable>& parameterSet) 
	{ 
		PythonObjectBuilder pyObBuilder;
		return pyObBuilder.setVariableTuple(parameterSet); 
	};
	
	std::string GetModuleEntryPoint() { 
		PythonModuleAPI moduleAPI;
		return moduleAPI.GetModuleEntryPoint(_moduleName); 
	};
	
	ot::ReturnMessage ExecuteCommand(const std::string& command)
	{
		return _api.Execute(command);
	}

	void SetupModule(const std::string& script);
private:
	PythonWrapper _wrapper;
	PythonAPI _api;
	const std::string _moduleName = "example";
};
