// @otlicense
// File: VariableToJSONConverter.cpp
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

#include "OTCore/VariableToJSONConverter.h"

ot::JsonValue ot::VariableToJSONConverter::operator()(const Variable& value, JsonAllocator& jsonAllocator)
{

	if (value.isInt32())
	{
		rapidjson::Value rJValue;
		rJValue.SetInt(value.getInt32());
		return rJValue;
	}
	else if (value.isInt64())
	{
		rapidjson::Value rJValue;
		rJValue.SetInt64(value.getInt64());
		return rJValue;
	}
	else if (value.isDouble())
	{
		rapidjson::Value rJValue;
		rJValue.SetDouble(value.getDouble());
		return rJValue;
	}
	else if (value.isFloat())
	{
		rapidjson::Value rJValue;
		rJValue.SetFloat(value.getFloat());
		return rJValue;
	}
	else if (value.isBool())
	{
		rapidjson::Value rJValue;
		rJValue.SetBool(value.getBool());
		return rJValue;
	}
	else if (value.isConstCharPtr())
	{
		rapidjson::Value rJValue;
		const char* temp = value.getConstCharPtr();
		rJValue.SetString(temp, jsonAllocator);
		return rJValue;
	}
	else if (value.isComplex())
	{
		const auto complexValue = value.getComplex();
		ot::JsonObject rJValue;
		rJValue.AddMember(rapidjson::StringRef(ot::g_realSerialiseKey.c_str()), complexValue.real(), jsonAllocator);
		rJValue.AddMember(rapidjson::StringRef(ot::g_ImagSerialiseKey.c_str()), complexValue.imag(), jsonAllocator);
		return rJValue;
	}
	else
	{
		throw std::exception("Not supported type for variable_t to rapidjson::Value conversion.");
	}
}

ot::JsonValue ot::VariableToJSONConverter::operator()(const std::list<Variable>& variables, JsonAllocator& allocator)
{
	ot::JsonArray jVariables;
		for (const ot::Variable& variable : variables)
		{
			jVariables.PushBack(operator()(variable, allocator), allocator);
		}
		return jVariables;
}
