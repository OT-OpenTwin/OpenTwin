// @otlicense
// File: ManagedPropertyObject.cpp
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

// OT header
#include "OTGui/PropertyManager.h"
#include "OTGui/ManagedPropertyObject.h"

ot::ManagedPropertyObject::ManagedPropertyObject(PropertyManager* _propertyManager) :
	m_manager(_propertyManager)
{

}

ot::ManagedPropertyObject::ManagedPropertyObject(ManagedPropertyObject&& _other) noexcept : m_manager(std::move(_other.m_manager)) {}

ot::ManagedPropertyObject& ot::ManagedPropertyObject::operator=(ManagedPropertyObject&& _other) noexcept {
	if (this != &_other) {
		m_manager = std::move(_other.m_manager);
	}
	return *this;
}

void ot::ManagedPropertyObject::addPropertiesToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	m_manager->addToJsonObject(_object, _allocator);
}

void ot::ManagedPropertyObject::setPropertiesFromJsonObject(const ot::ConstJsonObject& _object) {
	m_manager->setFromJsonObject(_object);
}

ot::Property* ot::ManagedPropertyObject::findProperty(const std::string& _groupName, const std::string& _valueName) {
	return m_manager->findProperty(_groupName, _valueName);
}

const ot::Property* ot::ManagedPropertyObject::findProperty(const std::string& _groupName, const std::string& _valueName) const {
	return m_manager->findProperty(_groupName, _valueName);
}
