// @otlicense
// File: ValueComparisionDefinition.cpp
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

#include "OTCore/ValueComparisionDefinition.h"

void ValueComparisionDefinition::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const
{
	_object.AddMember("Comperator", ot::JsonString(m_comparator, _allocator), _allocator);
	_object.AddMember("Name", ot::JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Value", ot::JsonString(m_value, _allocator), _allocator);
	_object.AddMember("Type", ot::JsonString(m_type, _allocator), _allocator);
	_object.AddMember("Unit", ot::JsonString(m_unit, _allocator), _allocator);
}

void ValueComparisionDefinition::setFromJsonObject(const ot::ConstJsonObject& _object)
{
	m_comparator = ot::json::getString(_object, "Comperator");
	m_name = ot::json::getString(_object, "Name");
	m_value = ot::json::getString(_object, "Value");
	m_type = ot::json::getString(_object, "Type");
	m_unit = ot::json::getString(_object, "Unit");
}
