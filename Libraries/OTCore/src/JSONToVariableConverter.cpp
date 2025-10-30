// @otlicense
// File: JSONToVariableConverter.cpp
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

#include "OTCore/JSONToVariableConverter.h"

ot::Variable ot::JSONToVariableConverter::operator()(const JsonValue& value)
{
	if (value.IsString())
	{
		return ot::Variable(value.GetString());
	}
	else if (value.IsInt())
	{
		return ot::Variable(value.GetInt());
	}
	else if (value.IsInt64())
	{
		return ot::Variable(value.GetInt64());
	}
	else if (value.IsFloat())
	{
		return ot::Variable(value.GetFloat());
	}
	else if (value.IsDouble())
	{
		return ot::Variable(value.GetDouble());
	}
	else if (value.IsBool())
	{
		return ot::Variable(value.GetBool());
	}
	else if (value.IsObject())
	{
		assert(value.HasMember(ot::g_ImagSerialiseKey.c_str()));
		assert(value.HasMember(ot::g_realSerialiseKey.c_str()));
		ot::complex var(value[ot::g_realSerialiseKey.c_str()].GetDouble(), value[ot::g_ImagSerialiseKey.c_str()].GetDouble());
		return ot::Variable(std::move(var));
	}
	else
	{
		throw std::exception("Not supported type for rapidjson::Value to variable_t conversion.");
	}
}

ot::Variable ot::JSONToVariableConverter::operator()(const JsonValue& value, const std::string _type)
{
	
	if (_type == ot::TypeNames::getStringTypeName())
	{
		if (value.IsString())
		{
			return ot::Variable(value.GetString());
		}
		else
		{ 
			throw std::exception("Cast to variant not possible due to type mismatch");
		}
	}
	else if (_type == ot::TypeNames::getInt32TypeName())
	{
		if (value.IsInt64() || value.IsInt())
		{
			return ot::Variable(value.GetInt());
		}
		else
		{
			throw std::exception("Cast to variant not possible due to type mismatch");
		}
	}
	else if (_type == ot::TypeNames::getInt64TypeName())
	{
		if (value.IsInt64() || value.IsInt())
		{
			return ot::Variable(value.GetInt64());
		}
		else
		{
			throw std::exception("Cast to variant not possible due to type mismatch");
		}
	}
	else if (_type == ot::TypeNames::getFloatTypeName())
	{
		if (value.IsNumber())
		{
			return ot::Variable(value.GetFloat());
		}
		else
		{
			throw std::exception("Cast to variant not possible due to type mismatch");
		}
	}
	else if (_type == ot::TypeNames::getDoubleTypeName())
	{
		if (value.IsNumber())
		{
			return ot::Variable(value.GetDouble());

		}
		else
		{
			throw std::exception("Cast to variant not possible due to type mismatch");
		}
	}
	else if (_type == ot::TypeNames::getBoolTypeName())
	{
		return ot::Variable(value.GetBool());
	}
	else
	{
		assert(false);
		throw std::exception("Cast to variant not possible due to unknown type name");
	}
}

std::list<ot::Variable> ot::JSONToVariableConverter::operator()(ot::ConstJsonArray& value)
{
	std::list<ot::Variable> variables;
	const rapidjson::SizeType arraySize = value.Size();

	for (rapidjson::SizeType i = 0; i < arraySize; i++)
	{
		const ot::Variable variable = operator()(value[i]);
		variables.push_back(variable);
	}
	return variables;
}

bool ot::JSONToVariableConverter::typeIsCompatible(const JsonValue& value, const std::string _type)
{
	auto type =	value.GetType();
	bool compatible = false;
	if (_type == ot::TypeNames::getStringTypeName())
	{
		compatible = value.IsString();
	}
	else if (_type == ot::TypeNames::getInt32TypeName())
	{		
		compatible = value.IsInt() || value.IsNumber();
	}
	else if (_type == ot::TypeNames::getInt64TypeName())
	{
		compatible =  value.IsInt64() || value.IsNumber();
	}
	else if (_type == ot::TypeNames::getFloatTypeName())
	{
		compatible = value.IsFloat() || value.IsNumber();
	}
	else if (_type == ot::TypeNames::getDoubleTypeName())
	{
		compatible = value.IsDouble() || value.IsNumber();
	}
	else if (_type == ot::TypeNames::getBoolTypeName())
	{
		compatible = value.IsBool();
	}
	else
	{
		assert(false);
	}
	return compatible;
}

