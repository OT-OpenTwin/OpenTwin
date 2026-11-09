// @otlicense
// File: VariableData.cpp
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

// OpenTwin header
#include "OTCore/Variable/VariableData.h"
#include "OTCore/Variable/JSONToVariableConverter.h"
#include "OTCore/Variable/VariableToJSONConverter.h"

ot::VariableData::VariableData(const ConstJsonObject& _jsonObject)
	: VariableData()
{
	setFromJsonObject(_jsonObject);
}

void ot::VariableData::addVariable(const std::string& _name, const Variable& _variable)
{
	OTAssert(!hasVariable(_name), "Variable already exists.");
	m_variables.insert_or_assign(_name, _variable);
}

bool ot::VariableData::hasVariable(const std::string& _name) const
{
	return m_variables.find(_name) != m_variables.end();
}

const ot::Variable& ot::VariableData::getVariable(const std::string& _name) const
{
	auto it = m_variables.find(_name);
	if (it == m_variables.end())
	{
		throw Exception::ObjectNotFound("Variable not found: \"" + _name + "\"");
	}
	else
	{
		return it->second;
	}
}

void ot::VariableData::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const
{
	VariableToJSONConverter converter;
	JsonArray arr;
	for (const auto& it : m_variables)
	{
		JsonObject varObj;
		varObj.AddMember("Name", JsonString(it.first, _allocator), _allocator);
		varObj.AddMember("Value", converter(it.second, _allocator), _allocator);
		arr.PushBack(std::move(varObj), _allocator);
	}

	_jsonObject.AddMember("Data", arr, _allocator);
}

void ot::VariableData::setFromJsonObject(const ConstJsonObject& _jsonObject)
{
	JSONToVariableConverter converter;
	m_variables.clear();

	const ConstJsonArray arr = json::getArray(_jsonObject, "Data");
	for (JsonSizeType i = 0; i < arr.Size(); ++i)
	{
		std::string name = json::getString(arr[i], "Name");
		Variable var = converter(arr[i]);
		m_variables.insert_or_assign(name, std::move(var));
	}
}
