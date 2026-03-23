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

#include "OTCore/QueryDescription/ValueComparisonDescription.h"

ot::ValueComparisonDescription::ValueComparisonDescription(const std::string& _name, const std::string& _comparator, const std::string& _value, const std::string& _type, const std::string& _unit)
	: m_name(_name), m_comparator(_comparator), m_value(_value)
{
	m_tupleDescription.setTupleUnits({ _unit });
	m_tupleDescription.setTupleElementDataTypes({ _type });
	m_tupleDescription.setTupleTypeName("");
	m_tupleDescription.setTupleFormatName("");
}

ot::ValueComparisonDescription::ValueComparisonDescription(const std::string& _name, const std::string& _comparator, const std::string& _value, TupleInstance _tupleInstance)
	: m_name(_name), m_comparator(_comparator), m_value(_value), m_tupleDescription(_tupleInstance)
{
}

void ot::ValueComparisonDescription::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const
{
	_object.AddMember("Comparator", ot::JsonString(m_comparator, _allocator), _allocator);
	_object.AddMember("Name", ot::JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Value", ot::JsonString(m_value, _allocator), _allocator);
	ot::JsonObject tupleDescription;
	m_tupleDescription.addToJsonObject(tupleDescription, _allocator);
	_object.AddMember("Tuple", tupleDescription, _allocator);
}

void ot::ValueComparisonDescription::setFromJsonObject(const ot::ConstJsonObject& _object)
{
	m_comparator = ot::json::getString(_object, "Comparator");
	m_name = ot::json::getString(_object, "Name");
	m_value = ot::json::getString(_object, "Value");
	ot::ConstJsonObject tupleObject = ot::json::getObject(_object, "Tuple");
	m_tupleDescription.setFromJsonObject(tupleObject);
}


void ot::ValueComparisonDescription::setTupleInstance(const TupleInstance& _tupleDescription)
{
	m_tupleDescription = _tupleDescription;
}

