//! @file NavigationTreeItem.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/NavigationTreeItem.h"

#define JSON_MEMBER_Text       "Text"
#define JSON_MEMBER_Flags      "Flags"
#define JSON_MEMBER_IconPath   "IconPath"
#define JSON_MEMBER_ChildItems "Childs"

ot::NavigationTreeItem::NavigationTreeItem() : m_flags(ot::NoNavigationItemFlags), m_parent(nullptr) {}

ot::NavigationTreeItem::NavigationTreeItem(const std::string& _text, ot::NavigationItemFlag _flags)
	: m_text(_text), m_flags(_flags), m_parent(nullptr)
{}

ot::NavigationTreeItem::NavigationTreeItem(const std::string& _text, const std::string& _iconPath, ot::NavigationItemFlag _flags)
	: m_text(_text), m_iconPath(_iconPath), m_flags(_flags), m_parent(nullptr)
{}

ot::NavigationTreeItem::NavigationTreeItem(const std::string& _text, const std::string& _iconPath, const std::list<NavigationTreeItem>& _childItems, ot::NavigationItemFlag _flags)
	: m_text(_text), m_iconPath(_iconPath), m_childs(_childItems), m_flags(_flags), m_parent(nullptr)
{
	for (NavigationTreeItem& c : m_childs) c.setParentNavigationTreeItem(this);
}

ot::NavigationTreeItem::NavigationTreeItem(const NavigationTreeItem& _other)
	: m_text(_other.m_text), m_iconPath(_other.m_iconPath), m_childs(_other.m_childs), m_flags(_other.m_flags), m_parent(nullptr)
{
	for (NavigationTreeItem& c : m_childs) c.setParentNavigationTreeItem(this);
}

ot::NavigationTreeItem::~NavigationTreeItem() {}

ot::NavigationTreeItem& ot::NavigationTreeItem::operator = (const NavigationTreeItem& _other) {
	m_text = _other.m_text;
	m_iconPath = _other.m_iconPath;
	m_childs = _other.m_childs;
	m_flags = _other.m_flags;

	for (NavigationTreeItem& c : m_childs) c.setParentNavigationTreeItem(this);

	return *this;
}

void ot::NavigationTreeItem::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember(JSON_MEMBER_Text, JsonString(m_text, _allocator), _allocator);
	_object.AddMember(JSON_MEMBER_IconPath, JsonString(m_iconPath, _allocator), _allocator);

	JsonArray childArr;
	for (const NavigationTreeItem& c : m_childs) {
		JsonObject childObj;
		c.addToJsonObject(childObj, _allocator);
		childArr.PushBack(childObj, _allocator);
	}
	_object.AddMember(JSON_MEMBER_ChildItems, childArr, _allocator);

	std::list<std::string> flagList = ot::toStringList(m_flags);

	JsonArray flagArr;
	for (const std::string& f : flagList) {
		flagArr.PushBack(JsonString(f, _allocator), _allocator);
	}
	_object.AddMember(JSON_MEMBER_Flags, flagArr, _allocator);
}

void ot::NavigationTreeItem::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_text = json::getString(_object, JSON_MEMBER_Text);
	m_iconPath = json::getString(_object, JSON_MEMBER_IconPath);

	m_childs.clear();
	ConstJsonArray childArr = json::getArray(_object, JSON_MEMBER_ChildItems);
	for (rapidjson::SizeType c = 0; c < childArr.Size(); c++) {
		ConstJsonObject childObj = json::getObject(childArr, c);
		NavigationTreeItem newItem;
		newItem.setFromJsonObject(childObj);
		m_childs.push_back(newItem);
	}

	std::list<std::string> flagList;
	ConstJsonArray flagArr = json::getArray(_object, JSON_MEMBER_Flags);
	for (rapidjson::SizeType c = 0; c < flagArr.Size(); c++) {
		flagList.push_back(json::getString(flagArr, c));
	}
	m_flags = stringListToNavigationItemFlags(flagList);
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
	if (this->text() != _other.text()) return;
	if (this->iconPath() != _other.iconPath()) {
		OT_LOG_WA("Icon path differs on merge. Ignoring");
	}

	std::list<NavigationTreeItem> bck = m_childs;
	m_childs.clear();
	for (const NavigationTreeItem& item : bck) {
		bool exists = false;
		for (NavigationTreeItem& eItem : m_childs) {
			if (eItem.text() == item.text()) {
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

std::string ot::NavigationTreeItem::itemPath(char _delimiter, const std::string& _suffix) {
	if (m_parent) {
		return m_parent->itemPath(_delimiter, _delimiter + m_text + _suffix);
	}
	else {
		return m_text + _suffix;
	}
}
