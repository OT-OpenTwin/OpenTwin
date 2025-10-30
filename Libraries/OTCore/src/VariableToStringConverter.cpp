// @otlicense
// File: VariableToStringConverter.cpp
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

#include "OTCore/VariableToStringConverter.h"
#include <cassert>

std::string ot::VariableToStringConverter::operator()(const ot::Variable& value)
{
	if (value.isConstCharPtr())
	{
		return value.getConstCharPtr();
	}
	else if (value.isBool())
	{
		std::string retVal;
		value.getBool() ? retVal = "TRUE" : retVal = "FALSE";
		return retVal;
	}
	else if (value.isDouble())
	{
		return std::to_string(value.getDouble());
	}
	else if (value.isFloat())
	{
		return std::to_string(value.getFloat());
	}
	else if (value.isInt32())
	{
		return std::to_string(value.getInt32());
	}
	else
	{
		assert(value.isInt64());
		return std::to_string(value.getInt64());
	}
}
