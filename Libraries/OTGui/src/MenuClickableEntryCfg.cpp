// @otlicense
// File: MenuClickableEntryCfg.cpp
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
#include "OTGui/MenuClickableEntryCfg.h"

ot::MenuClickableEntryCfg::MenuClickableEntryCfg() {

}

ot::MenuClickableEntryCfg::MenuClickableEntryCfg(const std::string& _name, const std::string& _text, const std::string& _iconPath)
	: m_name(_name), m_text(_text), m_iconPath(_iconPath) 
{

}

ot::MenuClickableEntryCfg::MenuClickableEntryCfg(const MenuClickableEntryCfg& _other)
	: MenuEntryCfg(_other), m_name(_other.m_name), m_text(_other.m_text), m_iconPath(_other.m_iconPath), m_toolTip(_other.m_toolTip)
{

}

ot::MenuClickableEntryCfg::~MenuClickableEntryCfg() {

}

ot::MenuClickableEntryCfg& ot::MenuClickableEntryCfg::operator=(const MenuClickableEntryCfg& _other) {
	if (this != &_other) {
		MenuEntryCfg::operator=(_other);

		m_name = _other.m_name;
		m_text = _other.m_text;
		m_iconPath = _other.m_iconPath;
		m_toolTip = _other.m_toolTip;
	}

	return *this;
}

void ot::MenuClickableEntryCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	MenuEntryCfg::addToJsonObject(_object, _allocator);

	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Text", JsonString(m_text, _allocator), _allocator);
	_object.AddMember("Icon", JsonString(m_iconPath, _allocator), _allocator);
	_object.AddMember("ToolTip", JsonString(m_toolTip, _allocator), _allocator);
}

void ot::MenuClickableEntryCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	MenuEntryCfg::setFromJsonObject(_object);

	m_name = json::getString(_object, "Name");
	m_text = json::getString(_object, "Text");
	m_iconPath = json::getString(_object, "Icon");
	m_toolTip = json::getString(_object, "ToolTip");
}
