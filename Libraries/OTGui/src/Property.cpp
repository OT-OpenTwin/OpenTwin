// @otlicense
// File: Property.cpp
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
#include "OTCore/LogDispatcher.h"
#include "OTGui/Property.h"
#include "OTGui/PropertyGroup.h"

ot::Property::Property(const Property* _other)
	: PropertyBase(*_other), m_parentGroup(nullptr)
{}

ot::Property::Property(const PropertyBase& _base) 
	: PropertyBase(_base), m_parentGroup(nullptr)
{}

ot::Property::Property(PropertyFlags _flags)
	: PropertyBase(_flags), m_parentGroup(nullptr)
{}

ot::Property::Property(const std::string& _name, PropertyFlags _flags)
	: PropertyBase(_name, _flags), m_parentGroup(nullptr)
{}

void ot::Property::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	PropertyBase::addToJsonObject(_object, _allocator);
	_object.AddMember(OT_JSON_MEMBER_Property_Type, JsonString(this->getPropertyType(), _allocator), _allocator);
	this->getPropertyData(_object, _allocator);
}

void ot::Property::setFromJsonObject(const ot::ConstJsonObject& _object) {
	PropertyBase::setFromJsonObject(_object);
	this->setPropertyData(_object); 
}

ot::Property::~Property() {
	if (m_parentGroup) {
		m_parentGroup->forgetProperty(this);
		delete m_parentGroup;
		m_parentGroup = nullptr;
	}
}

ot::Property* ot::Property::createCopyWithParents(void) const {
	Property* newProperty = this->createCopy();
	const PropertyGroup* oldParent = m_parentGroup;
	PropertyGroup* newParent = nullptr;
	while (oldParent) {
		PropertyGroup* tmp = oldParent->createCopy(false);
		if (newParent) {
			tmp->addChildGroup(newParent);
		}
		else {
			tmp->addProperty(newProperty);
		}
		newParent = tmp;
		oldParent = oldParent->getParentGroup();
	}

	return newProperty;
}

void ot::Property::mergeWith(const Property* _other, const MergeMode& _mergeMode) {
	PropertyBase::mergeWith(*_other, _mergeMode);
}

ot::PropertyGroup* ot::Property::getRootGroup(void) const {
	if (m_parentGroup) return m_parentGroup->getRootGroup();
	else return nullptr;
}

std::string ot::Property::getPropertyPath(char _delimiter) const {
	if (m_parentGroup) return m_parentGroup->getGroupPath() + _delimiter + this->getPropertyName();
	else return this->getPropertyName();
}