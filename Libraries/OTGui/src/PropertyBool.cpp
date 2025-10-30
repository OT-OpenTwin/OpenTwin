// @otlicense
// File: PropertyBool.cpp
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
#include "OTGui/PropertyBool.h"
#include "OTGui/PropertyFactoryRegistrar.h"

static ot::PropertyFactoryRegistrar<ot::PropertyBool> propertyBoolRegistrar(ot::PropertyBool::propertyTypeString());

ot::PropertyBool::PropertyBool(const PropertyBool* _other)
	: Property(_other), m_value(_other->m_value)
{}

ot::PropertyBool::PropertyBool(const PropertyBase& _base)
	: Property(_base), m_value(false)
{}

ot::PropertyBool::PropertyBool(PropertyFlags _flags)
	: Property(_flags), m_value(false)
{}

ot::PropertyBool::PropertyBool(bool _value, PropertyFlags _flags)
	: Property(_flags), m_value(_value)
{}

ot::PropertyBool::PropertyBool(const std::string& _name, bool _value, PropertyFlags _flags)
	: Property(_name, _flags), m_value(_value) 
{}

void ot::PropertyBool::mergeWith(const Property* _other, const MergeMode& _mergeMode) {
	Property::mergeWith(_other, _mergeMode);

	if (_mergeMode & PropertyBase::MergeValues) {
		const PropertyBool* other = dynamic_cast<const PropertyBool*>(_other);
		OTAssertNullptr(other);
		m_value = other->m_value;
	}
}

ot::Property* ot::PropertyBool::createCopy(void) const {
	return new PropertyBool(this);
}

void ot::PropertyBool::getPropertyData(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Value", m_value, _allocator);
}

void ot::PropertyBool::setPropertyData(const ot::ConstJsonObject& _object) {
	m_value = json::getBool(_object, "Value");
}