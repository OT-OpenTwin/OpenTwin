// @otlicense
// File: TreeWidgetItemInfo.cpp
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
#include "OTWidgets/IconManager.h"
#include "OTWidgets/TreeWidgetItemInfo.h"

ot::TreeWidgetItemInfo::TreeWidgetItemInfo() : m_flags(NavigationTreeItem::NoItemFlags) {}

ot::TreeWidgetItemInfo::TreeWidgetItemInfo(const QString& _text, const QIcon& _icon, const NavigationTreeItem::ItemFlags& _flags) :
	m_text(_text), m_icon(_icon), m_flags(_flags)
{}

ot::TreeWidgetItemInfo::TreeWidgetItemInfo(const NavigationTreeItem& _config) :
	m_text(QString::fromStdString(_config.getText())), m_flags(_config.getFlags())
{
	if (!_config.getVisibleIconPath().empty()) {
		m_icon = IconManager::getIcon(QString::fromStdString(_config.getVisibleIconPath()));
	}

	for (const NavigationTreeItem& child : _config.getChildItems()) {
		m_childs.push_back(TreeWidgetItemInfo(child));
	}
}

ot::TreeWidgetItemInfo::TreeWidgetItemInfo(const TreeWidgetItemInfo& _other) :
	m_text(_other.m_text), m_icon(_other.m_icon), m_flags(_other.m_flags), m_childs(_other.m_childs)
{}

ot::TreeWidgetItemInfo::~TreeWidgetItemInfo() {

}

ot::TreeWidgetItemInfo& ot::TreeWidgetItemInfo::operator = (const TreeWidgetItemInfo& _other) {
	m_text = _other.m_text;
	m_icon = _other.m_icon;
	m_flags = _other.m_flags;
	m_childs = _other.m_childs;
	return *this;
}

ot::TreeWidgetItemInfo& ot::TreeWidgetItemInfo::addChildItem(const QString& _text, const QIcon& _icon, const NavigationTreeItem::ItemFlags& _flags) {
	m_childs.push_back(TreeWidgetItemInfo(_text, _icon, _flags));
	return m_childs.back();
}

void ot::TreeWidgetItemInfo::addChildItem(const TreeWidgetItemInfo& _info) {
	m_childs.push_back(_info);
}

void ot::TreeWidgetItemInfo::clearChildItems(void) {
	m_childs.clear();
}
