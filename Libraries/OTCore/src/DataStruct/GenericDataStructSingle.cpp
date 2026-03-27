// @otlicense
// File: GenericDataStructSingle.cpp
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

#include "OTCore/DataStruct/GenericDataStructSingle.h"
#include "OTCore/Variable/JSONToVariableConverter.h"
#include "OTCore/Variable/VariableToJSONConverter.h"

static ot::GenericDataStruct::Registrar<ot::GenericDataStructSingle> registrar(ot::GenericDataStructSingle::className());

ot::GenericDataStructSingle::GenericDataStructSingle(const ot::Variable& _value)
	: m_value(_value)
{}

ot::GenericDataStructSingle::GenericDataStructSingle(ot::Variable&& _value) noexcept
	: m_value(std::move(_value))
{}

void ot::GenericDataStructSingle::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const
{
	GenericDataStruct::addToJsonObject(_object, _allocator);
	VariableToJSONConverter converter;
	_object.AddMember("Value", converter(m_value, _allocator), _allocator);
}

void ot::GenericDataStructSingle::setFromJsonObject(const ot::ConstJsonObject& _object)
{
	GenericDataStruct::setFromJsonObject(_object);
	JSONToVariableConverter converter;
	m_value = converter(_object["Value"]);
}

