// @otlicense
// File: DefaultPropertyValues.cpp
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
#include "OTGui/DefaultPropertyValues.h"

ot::DefaultPropertyValues::DefaultPropertyValues() {}

ot::DefaultPropertyValues::DefaultPropertyValues(const ConstJsonObject& _object) {
	this->setFromJsonObject(_object);
}

ot::DefaultPropertyValues::DefaultPropertyValues(DefaultPropertyValues&& _other) noexcept :
	m_defaultManager(std::move(_other.m_defaultManager)), m_objectToPropertiesMap(std::move(_other.m_objectToPropertiesMap))
{

}

ot::DefaultPropertyValues::~DefaultPropertyValues() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Operator

ot::DefaultPropertyValues& ot::DefaultPropertyValues::operator=(DefaultPropertyValues&& _other) noexcept {
	if (this != &_other) {
		m_defaultManager = std::move(_other.m_defaultManager);
		m_objectToPropertiesMap = std::move(_other.m_objectToPropertiesMap);
	}

	return *this;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Virtual methods

void ot::DefaultPropertyValues::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	JsonObject dObj;
	m_defaultManager.addToJsonObject(dObj, _allocator);

	JsonArray mapArr;
	for (const auto& it : m_objectToPropertiesMap) {
		JsonObject managerObj;
		it.second.addToJsonObject(managerObj, _allocator);

		JsonObject pairObj;
		pairObj.AddMember("K", JsonString(it.first, _allocator), _allocator);
		pairObj.AddMember("V", managerObj, _allocator);
		mapArr.PushBack(pairObj, _allocator);
	}

	_object.AddMember("Def", dObj, _allocator);
	_object.AddMember("Map", mapArr, _allocator);
}

void ot::DefaultPropertyValues::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_defaultManager.setFromJsonObject(json::getObject(_object, "Def"));
	
	m_objectToPropertiesMap.clear();
	for (const ConstJsonObject& pairObj : json::getObjectList(_object, "Map")) {
		m_objectToPropertiesMap.insert_or_assign(
			json::getString(pairObj, "K"),
			PropertyManager(json::getObject(pairObj, "V"))
		);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::DefaultPropertyValues::setObjectProperties(const std::string& _objectName, PropertyManager&& _properties) {
	m_objectToPropertiesMap.insert_or_assign(_objectName, std::move(_properties));
}

const ot::PropertyManager& ot::DefaultPropertyValues::getObjectProperties(const std::string& _objectName) const {
	const auto it = m_objectToPropertiesMap.find(_objectName);
	return (it == m_objectToPropertiesMap.end() ? m_defaultManager : it->second);
}
