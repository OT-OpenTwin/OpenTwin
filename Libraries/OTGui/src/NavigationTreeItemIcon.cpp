// @otlicense
// File: NavigationTreeItemIcon.cpp
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
#include "OTGui/NavigationTreeItemIcon.h"

ot::NavigationTreeItemIcon::NavigationTreeItemIcon() {}

ot::NavigationTreeItemIcon::NavigationTreeItemIcon(const std::string& _visibleIcon, const std::string& _hiddenIcon)
	: m_visibleIcon(_visibleIcon), m_hiddenIcon(_hiddenIcon)
{}

ot::NavigationTreeItemIcon::NavigationTreeItemIcon(const NavigationTreeItemIcon& _other) {
	*this = _other;
}

ot::NavigationTreeItemIcon::~NavigationTreeItemIcon() {}

ot::NavigationTreeItemIcon& ot::NavigationTreeItemIcon::operator = (const NavigationTreeItemIcon& _other) {
	if (this == &_other) return *this;

	m_visibleIcon = _other.m_visibleIcon;
	m_hiddenIcon = _other.m_hiddenIcon;

	return *this;
}

bool ot::NavigationTreeItemIcon::operator==(const NavigationTreeItemIcon& _other) const {
	return m_visibleIcon == _other.m_visibleIcon && m_hiddenIcon == _other.m_hiddenIcon;
}

bool ot::NavigationTreeItemIcon::operator!=(const NavigationTreeItemIcon& _other) const {
	return !(*this == _other);
}

void ot::NavigationTreeItemIcon::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Visible", JsonString(m_visibleIcon, _allocator), _allocator);
	_object.AddMember("Hidden", JsonString(m_hiddenIcon, _allocator), _allocator);
}

void ot::NavigationTreeItemIcon::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_visibleIcon = json::getString(_object, "Visible");
	m_hiddenIcon = json::getString(_object, "Hidden");
}
