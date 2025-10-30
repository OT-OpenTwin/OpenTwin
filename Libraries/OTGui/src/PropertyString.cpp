// @otlicense
// File: PropertyString.cpp
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
#include "OTGui/PropertyString.h"
#include "OTGui/PropertyFactoryRegistrar.h"

static ot::PropertyFactoryRegistrar<ot::PropertyString> propertyStringRegistrar(ot::PropertyString::propertyTypeString());

ot::PropertyString::PropertyString(const PropertyString* _other) 
	: Property(_other), m_value(_other->m_value), m_placeholderText(_other->m_placeholderText), m_maxLength(_other->m_maxLength)
{}

ot::PropertyString::PropertyString(const PropertyBase & _base)
	: Property(_base), m_maxLength(0)
{}

ot::PropertyString::PropertyString(PropertyFlags _flags)
	: Property(_flags), m_maxLength(0)
{}

ot::PropertyString::PropertyString(const std::string& _value, PropertyFlags _flags)
	: Property(_flags), m_value(_value), m_maxLength(0)
{}

ot::PropertyString::PropertyString(const std::string& _name, const std::string& _value, PropertyFlags _flags)
	: Property(_name, _flags), m_value(_value), m_maxLength(0)
{}

void ot::PropertyString::mergeWith(const Property* _other, const MergeMode& _mergeMode) {
	Property::mergeWith(_other, _mergeMode);

	const PropertyString* other = dynamic_cast<const PropertyString*>(_other);
	OTAssertNullptr(other);

	if (_mergeMode & PropertyBase::MergeValues) {
		m_value = other->m_value;
	}
	if (_mergeMode & PropertyBase::MergeConfig) {
		m_placeholderText = other->m_placeholderText;
		m_maxLength = other->m_maxLength;
	}
}

ot::Property* ot::PropertyString::createCopy(void) const {
	return new PropertyString(this);
}

void ot::PropertyString::getPropertyData(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Value", JsonString(m_value, _allocator), _allocator);
	_object.AddMember("Placeholder", JsonString(m_placeholderText, _allocator), _allocator);
	_object.AddMember("MaxLength", m_maxLength, _allocator);
}

void ot::PropertyString::setPropertyData(const ot::ConstJsonObject& _object) {
	m_value = json::getString(_object, "Value");
	m_placeholderText = json::getString(_object, "Placeholder");
	m_maxLength = json::getUInt(_object, "MaxLength");
}