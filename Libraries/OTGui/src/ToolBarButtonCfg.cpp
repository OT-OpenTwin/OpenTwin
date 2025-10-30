// @otlicense
// File: ToolBarButtonCfg.cpp
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
#include "OTGui/ToolBarButtonCfg.h"

ot::ToolBarButtonCfg::ToolBarButtonCfg(const std::string& _page, const std::string& _group, const ToolButtonCfg& _button)
	: m_page(_page), m_group(_group), m_button(_button) {}

ot::ToolBarButtonCfg::ToolBarButtonCfg(const std::string& _page, const std::string& _group, const std::string& _buttonText, const std::string& _buttonIcon)
	: m_page(_page), m_group(_group), m_button(_buttonText, _buttonText, _buttonIcon) {}

ot::ToolBarButtonCfg::ToolBarButtonCfg(const std::string& _page, const std::string& _group, const std::string& _subGroup, const ToolButtonCfg& _button)
	: m_page(_page), m_group(_group), m_subGroup(_subGroup), m_button(_button) {}

ot::ToolBarButtonCfg::ToolBarButtonCfg(const std::string& _page, const std::string& _group, const std::string& _subGroup, const std::string& _buttonText, const std::string& _buttonIcon) 
	: m_page(_page), m_group(_group), m_subGroup(_subGroup), m_button(_buttonText, _buttonText, _buttonIcon) {}

// ###########################################################################################################################################################################################################################################################################################################################

// Virtual methods

void ot::ToolBarButtonCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Page", JsonString(m_page, _allocator), _allocator);
	_object.AddMember("Group", JsonString(m_group, _allocator), _allocator);
	_object.AddMember("SubGroup", JsonString(m_subGroup, _allocator), _allocator);
	_object.AddMember("Button", JsonObject(m_button, _allocator), _allocator);
}

void ot::ToolBarButtonCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_page = json::getString(_object, "Page");
	m_group = json::getString(_object, "Group");
	m_subGroup = json::getString(_object, "SubGroup");
	m_button.setFromJsonObject(json::getObject(_object, "Button"));
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

std::string ot::ToolBarButtonCfg::getFullPath() const {
	return m_page + ":" + m_group + (m_subGroup.empty() ? std::string() : (":" + m_subGroup)) + ":" + m_button.getName();
}