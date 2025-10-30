// @otlicense
// File: ParametricCombination.h
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

#include <map>
#include <string>

class ParametricCombination
{
public:
	ParametricCombination() {};
	~ParametricCombination() {};

	void addParameter(const std::string& parameterName, double parameterValue) { m_parameters[parameterName] = parameterValue; }
	const std::map<std::string, double>& getParameters() { return m_parameters; }

private:
	std::map<std::string, double> m_parameters;
};
