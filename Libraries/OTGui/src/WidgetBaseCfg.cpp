// @otlicense
// File: WidgetBaseCfg.cpp
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
#include "OTGui/WidgetBaseCfg.h"

ot::WidgetBaseCfg::WidgetBaseCfg() : m_lockFlags(LockType::All) {}

ot::WidgetBaseCfg::WidgetBaseCfg(const std::string& _name)
	: m_lockFlags(LockType::All), m_name(_name) {}

ot::WidgetBaseCfg::~WidgetBaseCfg() {}

void ot::WidgetBaseCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("ToolTip", JsonString(m_toolTip, _allocator), _allocator);
	_object.AddMember("LockFlags", JsonArray(toStringList(m_lockFlags), _allocator), _allocator);
}

void ot::WidgetBaseCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_name = json::getString(_object, "Name");
	m_toolTip = json::getString(_object, "ToolTip");
	m_lockFlags = stringListToLockTypes(json::getStringList(_object, "LockFlags"));
}
