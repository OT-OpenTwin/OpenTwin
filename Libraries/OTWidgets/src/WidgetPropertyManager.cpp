// @otlicense
// File: WidgetPropertyManager.cpp
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
#include "OTSystem/OTAssert.h"
#include "OTWidgets/ManagedPropertyLink.h"
#include "OTWidgets/WidgetPropertyManager.h"
#include "OTWidgets/ManagedWidgetPropertyObject.h"

ot::WidgetPropertyManager::WidgetPropertyManager(ManagedWidgetPropertyObject* _object) :
	m_object(_object)
{
	
}

ot::WidgetPropertyManager::WidgetPropertyManager(WidgetPropertyManager&& _other) noexcept :
	PropertyManager(std::move(_other)), m_links(std::move(_other.m_links)), m_object(std::move(_other.m_object))
{

}

ot::WidgetPropertyManager& ot::WidgetPropertyManager::operator=(WidgetPropertyManager&& _other) noexcept {
	if (this != &_other) {
		m_links = std::move(_other.m_links);
		m_object = std::move(_other.m_object);
	}

	PropertyManager::operator=(std::move(_other));
	return *this;
}

void ot::WidgetPropertyManager::propertyChanged(const Property* _property) {
	PropertyManager::propertyChanged(_property);
}

void ot::WidgetPropertyManager::readingProperty(const std::string& _propertyGroupName, const std::string& _propertyName) const {
	PropertyManager::readingProperty(_propertyGroupName, _propertyName);
}

void ot::WidgetPropertyManager::addPropertyLink(ManagedPropertyLink* _link) {
	OTAssertNullptr(_link);
	OTAssert(std::find(m_links.begin(), m_links.end(), _link) == m_links.end(), "Link already registered");
	m_links.push_back(_link);
}

void ot::WidgetPropertyManager::forgetPropertyLink(ManagedPropertyLink* _link) {
	auto it = std::find(m_links.begin(), m_links.end(), _link);
	if (it != m_links.end()) {
		m_links.erase(it);
	}
}
