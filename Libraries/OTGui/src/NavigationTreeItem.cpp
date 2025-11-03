// @otlicense
// File: NavigationTreeItem.cpp
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
#include "OTGui/NavigationTreeItem.h"

ot::NavigationTreeItem::NavigationTreeItem() : m_flags(NoItemFlags), m_parent(nullptr) {}

ot::NavigationTreeItem::NavigationTreeItem(const std::string& _text, const ItemFlags& _flags)
	: m_text(_text), m_flags(_flags), m_parent(nullptr)
{}

ot::NavigationTreeItem::NavigationTreeItem(const std::string& _text, const std::string& _iconPath, const ItemFlags& _flags)
	: m_text(_text), m_icon(_iconPath, _iconPath), m_flags(_flags), m_parent(nullptr)
{}

ot::NavigationTreeItem::NavigationTreeItem(const std::string& _text, const std::string& _iconPath, const std::list<NavigationTreeItem>& _childItems, const ItemFlags& _flags)
	: m_text(_text), m_icon(_iconPath, _iconPath), m_childs(_childItems), m_flags(_flags), m_parent(nullptr)
{
	for (NavigationTreeItem& c : m_childs) c.setParentNavigationTreeItem(this);
}

ot::NavigationTreeItem::NavigationTreeItem(const NavigationTreeItem& _other)
	: m_text(_other.m_text), m_icon(_other.m_icon), m_childs(_other.m_childs), m_flags(_other.m_flags), m_parent(nullptr)
{
	for (NavigationTreeItem& c : m_childs) {
		c.setParentNavigationTreeItem(this);
	}
}

ot::NavigationTreeItem::~NavigationTreeItem() {}

ot::NavigationTreeItem& ot::NavigationTreeItem::operator = (const NavigationTreeItem& _other) {
	m_text = _other.m_text;
	m_icon = _other.m_icon;
	m_childs = _other.m_childs;
	m_flags = _other.m_flags;

	for (NavigationTreeItem& c : m_childs) {
		c.setParentNavigationTreeItem(this);
	}

	return *this;
}

void ot::NavigationTreeItem::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Text", JsonString(m_text, _allocator), _allocator);
	_object.AddMember("Icon", JsonObject(m_icon, _allocator), _allocator);

	JsonArray childArr;
	for (const NavigationTreeItem& c : m_childs) {
		childArr.PushBack(JsonObject(c, _allocator), _allocator);
	}
	_object.AddMember("Childs", childArr, _allocator);
	_object.AddMember("Flags", static_cast<uint64_t>(m_flags), _allocator);
}

void ot::NavigationTreeItem::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_text = json::getString(_object, "Text");
	m_icon.setFromJsonObject(json::getObject(_object, "Icon"));

	m_childs.clear();
	for (const ConstJsonObject& childObj : json::getObjectList(_object, "Childs")) {
		NavigationTreeItem newItem;
		newItem.setFromJsonObject(childObj);
		m_childs.push_back(newItem);
	}

	m_flags = static_cast<ItemFlag>(json::getUInt64(_object, "Flags"));
}

void ot::NavigationTreeItem::addChildItem(const NavigationTreeItem& _item) {
	m_childs.push_back(_item);
	m_childs.back().setParentNavigationTreeItem(this);
}

void ot::NavigationTreeItem::setChildItems(const std::list<NavigationTreeItem>& _items) {
	m_childs = _items;
	for (NavigationTreeItem& c : m_childs) c.setParentNavigationTreeItem(this);
}

void ot::NavigationTreeItem::merge(const NavigationTreeItem& _other) {
	// Merge own childs
	std::list<NavigationTreeItem> bck = m_childs;
	m_childs.clear();
	for (const NavigationTreeItem& item : bck) {
		bool exists = false;
		for (NavigationTreeItem& eItem : m_childs) {
			if (eItem.m_text == item.m_text) {
				eItem.merge(item);
				exists = true;
				break;
			}
		}
		if (!exists) {
			m_childs.push_back(item);
			m_childs.back().setParentNavigationTreeItem(this);
		}
	}

	if (m_text != _other.m_text) return;
	if (m_icon != _other.m_icon) {
		OT_LOG_WA("Icon path differs on merge. Ignoring");
	}

	// Merge with other
	for (const NavigationTreeItem& item : _other.m_childs) {
		bool exists = false;
		for (NavigationTreeItem& eItem : m_childs) {
			if (eItem.m_text == item.m_text) {
				eItem.merge(item);
				exists = true;
				break;
			}
		}
		if (!exists) {
			m_childs.push_back(item);
			m_childs.back().setParentNavigationTreeItem(this);
		}
	}
}

std::string ot::NavigationTreeItem::itemPath(char _delimiter, const std::string& _suffix) const {
	if (m_parent) {
		return m_parent->itemPath(_delimiter, _delimiter + m_text + _suffix);
	}
	else {
		return m_text + _suffix;
	}
}

bool ot::NavigationTreeItem::filter(const ItemFlags& _flags) {
	std::list<NavigationTreeItem> bck = m_childs;
	m_childs.clear();
	for (NavigationTreeItem& itm : bck) {
		if (itm.filter(_flags)) {
			m_childs.push_back(itm);
			m_childs.back().setParentNavigationTreeItem(this);
		}
	}
	return (m_flags & _flags) || !m_childs.empty();
}