// @otlicense
// File: VariableToBSONStringConverter.cpp
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

#include "OTCore/VariableToBSONStringConverter.h"
#include <cassert>

std::string ot::VariableToBSONStringConverter::operator()(const ot::Variable& variable)
{
	std::string bsonString;
	if (variable.isConstCharPtr())
	{
		bsonString = "\"" + std::string(variable.getConstCharPtr()) + "\"";
	}
	else
	{
		if (variable.isDouble())
		{
			bsonString = std::to_string(variable.getDouble());
		}
		else if (variable.isFloat())
		{
			bsonString = std::to_string(variable.getFloat());
		}
		else if (variable.isInt64())
		{
			bsonString = std::to_string(variable.getInt64());
		}
		else if (variable.isInt32())
		{
			bsonString = std::to_string(variable.getInt32());
		}
		else if (variable.isBool())
		{
			if (variable.getBool())
			{
				bsonString ="true";
			}
			else
			{
				bsonString = "false";
			}
		}
		else
		{
			assert(0);
		}
	}
    return bsonString;
}
