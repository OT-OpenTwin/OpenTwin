// @otlicense
// File: GenericDataStructVector.cpp
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
#include "OTCore/DataStruct/GenericDataStructVector.h"
#include "OTCore/Variable/VariableToJSONConverter.h"
#include "OTCore/Variable/JSONToVariableConverter.h"

static ot::GenericDataStruct::Registrar<ot::GenericDataStructVector> registrar(ot::GenericDataStructVector::className());

ot::GenericDataStructVector::GenericDataStructVector(size_t _numberOfEntries)
{
	m_values.resize(_numberOfEntries, ot::Variable());
}

ot::GenericDataStructVector::GenericDataStructVector(const std::vector<ot::Variable>& _values)
	: m_values(_values)
{}

ot::GenericDataStructVector::GenericDataStructVector(std::vector<ot::Variable>&& _values) noexcept
	: m_values(std::move(_values))
{}

void ot::GenericDataStructVector::setValue(uint32_t _index, const ot::Variable& _value)
{
	 m_values[_index] = _value; 
}

void ot::GenericDataStructVector::setValue(uint32_t _index, ot::Variable&& _value)
{
	m_values[_index] = std::move(_value); 
}

void ot::GenericDataStructVector::setValues(const std::vector<ot::Variable>& _values)
{
	m_values = _values;
}

void ot::GenericDataStructVector::setValues(std::vector<ot::Variable>&& _values)
{
	m_values = std::move(_values);
}

void ot::GenericDataStructVector::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const
{
	VariableToJSONConverter converter;
	GenericDataStruct::addToJsonObject(_object, _allocator);
	ot::JsonArray arr;
	for (auto& value: m_values)
	{
		arr.PushBack(converter(value, _allocator), _allocator);
	}
	_object.AddMember("values", arr, _allocator);
}

void ot::GenericDataStructVector::setFromJsonObject(const ot::ConstJsonObject& _object)
{
	GenericDataStruct::setFromJsonObject(_object);
	JSONToVariableConverter converter;

	const ConstJsonArray array = ot::json::getArray(_object, "values");
	m_values.resize(array.Size(), ot::Variable());
	for (uint32_t j = 0; j < array.Size(); j++)
	{
		m_values[j] = converter(array[j]);
	}
}
