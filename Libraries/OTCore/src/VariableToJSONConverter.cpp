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
#include "OTCore/ComplexNumbers/SerialisationKeys.h"

ot::JsonValue ot::VariableToJSONConverter::operator()(const Variable& value, JsonAllocator& jsonAllocator)
{

	if (value.isInt32())
	{
		ot::JsonValue rJValue;
		rJValue.SetInt(value.getInt32());
		return rJValue;
	}
	else if (value.isInt64())
	{
		ot::JsonValue rJValue;
		rJValue.SetInt64(value.getInt64());
		return rJValue;
	}
	else if (value.isDouble())
	{
		ot::JsonValue rJValue;
		rJValue.SetDouble(value.getDouble());
		return rJValue;
	}
	else if (value.isFloat())
	{
		ot::JsonValue rJValue;
		rJValue.SetFloat(value.getFloat());
		return rJValue;
	}
	else if (value.isBool())
	{
		ot::JsonValue rJValue;
		rJValue.SetBool(value.getBool());
		return rJValue;
	}
	else if (value.isConstCharPtr())
	{
		ot::JsonValue rJValue;
		const char* temp = value.getConstCharPtr();
		rJValue.SetString(temp, jsonAllocator);
		return rJValue;
	}
	else if (value.isComplex())
	{
		const auto complexValue = value.getComplex();
		ot::JsonObject rJValue;
		ot::JsonString realKey(ot::ComplexNumbers::SerialisationKeys::g_real, jsonAllocator);
		rJValue.AddMember(realKey, complexValue.imag(), jsonAllocator);
		ot::JsonString imagKey(ot::ComplexNumbers::SerialisationKeys::g_imag, jsonAllocator);
		rJValue.AddMember(imagKey, complexValue.imag(), jsonAllocator);
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
