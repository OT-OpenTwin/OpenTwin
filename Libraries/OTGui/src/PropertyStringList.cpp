// @otlicense
// File: PropertyStringList.cpp
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
#include "OTGui/PropertyStringList.h"
#include "OTGui/PropertyFactoryRegistrar.h"

static ot::PropertyFactoryRegistrar<ot::PropertyStringList> propertyStringListRegistrar(ot::PropertyStringList::propertyTypeString());

ot::PropertyStringList::PropertyStringList(const PropertyStringList* _other) 
	: Property(_other), m_current(_other->m_current), m_list(_other->m_list)
{}

ot::PropertyStringList::PropertyStringList(const PropertyBase & _base)
	: Property(_base)
{}

ot::PropertyStringList::PropertyStringList(PropertyFlags _flags)
	: Property(_flags) 
{}

ot::PropertyStringList::PropertyStringList(const std::string& _current, PropertyFlags _flags)
	: Property(_flags), m_current(_current) 
{}

ot::PropertyStringList::PropertyStringList(const std::string& _current, const std::list<std::string>& _list, PropertyFlags _flags)
	: Property(_flags), m_current(_current), m_list(_list) 
{}

ot::PropertyStringList::PropertyStringList(const std::string& _current, const std::vector<std::string>& _list, PropertyFlags _flags)
	: Property(_flags), m_current(_current)
{
	for (const std::string& s : _list) {
		m_list.push_back(s);
	}
}

ot::PropertyStringList::PropertyStringList(const std::string& _name, const std::string& _current, const std::list<std::string>& _list, PropertyFlags _flags)
	: Property(_name, _flags), m_current(_current), m_list(_list)
{}

ot::PropertyStringList::PropertyStringList(const std::string& _name, const std::string& _current, const std::vector<std::string>& _list, PropertyFlags _flags)
	: Property(_name, _flags), m_current(_current)
{
	for (const std::string& s : _list) {
		m_list.push_back(s);
	}
}

void ot::PropertyStringList::mergeWith(const Property* _other, const MergeMode& _mergeMode) {
	Property::mergeWith(_other, _mergeMode);

	const PropertyStringList* other = dynamic_cast<const PropertyStringList*>(_other);
	OTAssertNullptr(other);

	if (_mergeMode & PropertyBase::MergeValues) {	
		m_current = other->m_current;
	}
	if (_mergeMode & PropertyBase::MergeConfig) {
		m_list = other->m_list;
	}
}

ot::Property* ot::PropertyStringList::createCopy(void) const {
	return new PropertyStringList(this);
}

void ot::PropertyStringList::getPropertyData(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("List", JsonArray(m_list, _allocator), _allocator);
	_object.AddMember("Current", JsonString(m_current, _allocator), _allocator);
}

void ot::PropertyStringList::setPropertyData(const ot::ConstJsonObject& _object) {
	m_list = json::getStringList(_object, "List");
	m_current = json::getString(_object, "Current");
}