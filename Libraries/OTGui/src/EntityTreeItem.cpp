// @otlicense
// File: EntityTreeItem.cpp
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
#include "OTGui/EntityTreeItem.h"

ot::EntityTreeItem::EntityTreeItem()
	: m_iconsChanged(false), 
	m_isEditableChanged(false), m_isEditable(false),
	m_selectChildsChanged(false), m_selectChilds(false)
{}

ot::EntityTreeItem::EntityTreeItem(const BasicEntityInformation& _entityInfo)
	: EntityTreeItem()
{
	BasicEntityInformation::operator=(_entityInfo);
}

bool ot::EntityTreeItem::operator==(const EntityTreeItem& _other) const {
	return BasicEntityInformation::operator==(_other)
		&& (m_icons == _other.m_icons)
		&& (m_isEditable == _other.m_isEditable)
		&& (m_selectChilds == _other.m_selectChilds);
}

bool ot::EntityTreeItem::operator!=(const EntityTreeItem& _other) const {
	return !(*this == _other);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Public: Virtual methods

void ot::EntityTreeItem::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	BasicEntityInformation::addToJsonObject(_object, _allocator);

	_object.AddMember("Icons", JsonObject(m_icons, _allocator), _allocator);
	_object.AddMember("IsEditable", m_isEditable, _allocator);
	_object.AddMember("SelectChilds", m_selectChilds, _allocator);
}

void ot::EntityTreeItem::setFromJsonObject(const ot::ConstJsonObject& _object) {
	BasicEntityInformation::setFromJsonObject(_object);
	
	m_icons.setFromJsonObject(json::getObject(_object, "Icons"));
	m_isEditable = json::getBool(_object, "IsEditable");
	m_selectChilds = json::getBool(_object, "SelectChilds");
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

void ot::EntityTreeItem::setVisibleIcon(const std::string& _visibleIcon) {
	if (m_icons.getVisibleIcon() != _visibleIcon) {
		m_icons.setVisibleIcon(_visibleIcon);
		m_iconsChanged = true;
	}
}

void ot::EntityTreeItem::setHiddenIcon(const std::string& _hiddenIcon) {
	if (m_icons.getHiddenIcon() != _hiddenIcon) {
		m_icons.setHiddenIcon(_hiddenIcon);
		m_iconsChanged = true;
	}
}

void ot::EntityTreeItem::setIcons(const NavigationTreeItemIcon& _icons) {
	if (m_icons != _icons) {
		m_icons = _icons;
		m_iconsChanged = true;
	}
}

void ot::EntityTreeItem::setIsEditable(bool _isEditable) {
	if (m_isEditable != _isEditable) {
		m_isEditable = _isEditable;
		m_isEditableChanged = true;
	}
}

void ot::EntityTreeItem::setSelectChilds(bool _selectChilds) {
	if (m_selectChilds != _selectChilds) {
		m_selectChilds = _selectChilds;
		m_selectChildsChanged = true;
	}
}
