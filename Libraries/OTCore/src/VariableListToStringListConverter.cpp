// @otlicense
// File: VariableListToStringListConverter.cpp
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

#include "OTCore/VariableListToStringListConverter.h"

std::list<std::string> ot::VariableListToStringListConverter::operator()(const std::list<Variable>& values)
{
	std::list<std::string> strValueList;
	for (const ot::Variable& value : values)
	{
		if (value.isConstCharPtr())
		{
			strValueList.push_back(value.getConstCharPtr());
		}
	}

	return strValueList;
}
