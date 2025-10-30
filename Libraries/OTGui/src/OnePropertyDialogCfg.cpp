// @otlicense
// File: OnePropertyDialogCfg.cpp
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
#include "OTGui/PropertyFactory.h"
#include "OTGui/OnePropertyDialogCfg.h"

ot::OnePropertyDialogCfg::OnePropertyDialogCfg(Property* _property) : m_property(_property) {}

ot::OnePropertyDialogCfg::OnePropertyDialogCfg(const std::string& _title, Property* _property) : DialogCfg(_title), m_property(_property) {}

ot::OnePropertyDialogCfg::~OnePropertyDialogCfg() {
	if (m_property) delete m_property;
	m_property = nullptr;
}

void ot::OnePropertyDialogCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	DialogCfg::addToJsonObject(_object, _allocator);

	if (m_property) {
		JsonObject propertyObj;
		m_property->addToJsonObject(propertyObj, _allocator);
		_object.AddMember("Property", propertyObj, _allocator);
	}
	else {
		OT_LOG_EA("Property not set");
		JsonNullValue propertyObj;
		_object.AddMember("Property", propertyObj, _allocator);
	}
}

void ot::OnePropertyDialogCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	DialogCfg::setFromJsonObject(_object);
	if (m_property) delete m_property;
	m_property = nullptr;
	
	if (json::isObject(_object, "Property")) {
		ConstJsonObject propertyObj = json::getObject(_object, "Property");
		m_property = PropertyFactory::createProperty(propertyObj);
	}
}

void ot::OnePropertyDialogCfg::setProperty(Property* _property) {
	if (m_property) delete m_property;
	m_property = _property;
}

ot::Property* ot::OnePropertyDialogCfg::takeProperty(void) {
	Property* p = m_property;
	m_property = nullptr;
	return p;
}