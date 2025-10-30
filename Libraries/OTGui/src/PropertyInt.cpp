// @otlicense
// File: PropertyInt.cpp
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
#include "OTCore/JSON.h"
#include "OTCore/LogDispatcher.h"
#include "OTGui/PropertyInt.h"
#include "OTGui/PropertyFactoryRegistrar.h"

static ot::PropertyFactoryRegistrar<ot::PropertyInt> propertyIntRegistrar(ot::PropertyInt::propertyTypeString());

ot::PropertyInt::PropertyInt(const PropertyInt* _other) 
	: Property(_other), m_min(_other->m_min), m_max(_other->m_max), m_value(_other->m_value), m_suffix(_other->m_suffix)
{}

ot::PropertyInt::PropertyInt(const PropertyBase& _base)
	: Property(_base), m_value(0), m_min(INT_MIN), m_max(INT_MAX)
{}

ot::PropertyInt::PropertyInt(PropertyFlags _flags) 
	: Property(_flags), m_value(0), m_min(INT_MIN), m_max(INT_MAX)
{}

ot::PropertyInt::PropertyInt(int _value, PropertyFlags _flags)
	: Property(_flags), m_value(_value), m_min(INT_MIN), m_max(INT_MAX)
{}

ot::PropertyInt::PropertyInt(const std::string& _name, int _value, PropertyFlags _flags) 
	: Property(_name, _flags), m_value(_value), m_min(INT_MIN), m_max(INT_MAX)
{}

ot::PropertyInt::PropertyInt(const std::string& _name, int _value, int _min, int _max, PropertyFlags _flags)
	: Property(_name, _flags), m_value(_value), m_min(_min), m_max(_max)
{}

void ot::PropertyInt::mergeWith(const Property* _other, const MergeMode& _mergeMode) {
	Property::mergeWith(_other, _mergeMode);

	const PropertyInt* other = dynamic_cast<const PropertyInt*>(_other);
	OTAssertNullptr(other);

	if (_mergeMode & PropertyBase::MergeValues) {
		m_value = other->m_value;
	}
	if (_mergeMode & PropertyBase::MergeConfig) {
		m_min = other->m_min;
		m_max = other->m_max;
		m_suffix = other->m_suffix;
	}
}

ot::Property* ot::PropertyInt::createCopy(void) const {
	return new PropertyInt(this);
}

void ot::PropertyInt::getPropertyData(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Value", m_value, _allocator);
	_object.AddMember("Min", m_min, _allocator);
	_object.AddMember("Max", m_max, _allocator);
	_object.AddMember("Suffix", JsonString(m_suffix, _allocator), _allocator);
}

void ot::PropertyInt::setPropertyData(const ot::ConstJsonObject& _object) {
	m_value = json::getInt(_object, "Value");
	m_min = json::getInt(_object, "Min");
	m_max = json::getInt(_object, "Max");
	if (_object.HasMember("Suffix")) {
		m_suffix = json::getString(_object, "Suffix");
	}
}