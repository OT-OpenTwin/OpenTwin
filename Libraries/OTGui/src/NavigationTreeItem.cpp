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

std::string ot::toString(NavigationTreeItemFlag _flag) {
	switch (_flag)
	{
	case ot::NavigationTreeItemFlag::NoNavigationTreeItemFlags: return "<null>";
	case ot::NavigationTreeItemFlag::RemoveItemWhenEmpty: return "RemoveItemWhenEmpty";
	case ot::NavigationTreeItemFlag::ItemMayBeAdded: return "ItemMayBeAdded";
	case ot::NavigationTreeItemFlag::ItemIsSelected: return "ItemIsSelected";
	default:
		OT_LOG_EA("Unknown navigation tree item flag");
		return "<null>";
	}
}

std::list<std::string> ot::toStringList(NavigationTreeItemFlag _flags) {
	std::list<std::string> ret;
	if (_flags & ot::RemoveItemWhenEmpty) ret.push_back(toString(ot::RemoveItemWhenEmpty));
	if (_flags & ot::ItemMayBeAdded) ret.push_back(toString(ot::ItemMayBeAdded));
	if (_flags & ot::ItemIsSelected) ret.push_back(toString(ot::ItemIsSelected));

	return ret;
}

ot::NavigationTreeItemFlag ot::stringToNavigationItemFlag(const std::string& _flag) {
	if (_flag == ot::toString(NavigationTreeItemFlag::RemoveItemWhenEmpty)) return NavigationTreeItemFlag::RemoveItemWhenEmpty;
	else if (_flag == ot::toString(NavigationTreeItemFlag::ItemMayBeAdded)) return NavigationTreeItemFlag::ItemMayBeAdded;
	else if (_flag == ot::toString(NavigationTreeItemFlag::ItemIsSelected)) return NavigationTreeItemFlag::ItemIsSelected;
	else {
		OT_LOG_EA("Unknown navigation tree item flag");
		return NoNavigationTreeItemFlags;
	}
}

ot::NavigationTreeItemFlag ot::stringListToNavigationItemFlags(const std::list<std::string>& _flags) {
	NavigationTreeItemFlag ret = ot::NoNavigationTreeItemFlags;
	for (const std::string& flag : _flags) {
		ret |= stringToNavigationItemFlag(flag);
	}
	return ret;
}

ot::NavigationTreeItem::NavigationTreeItem() : m_flags(ot::NoNavigationTreeItemFlags), m_parent(nullptr) {}

ot::NavigationTreeItem::NavigationTreeItem(const std::string& _text, ot::NavigationTreeItemFlag _flags)
	: m_text(_text), m_flags(_flags), m_parent(nullptr)
{}

ot::NavigationTreeItem::NavigationTreeItem(const std::string& _text, const std::string& _iconPath, ot::NavigationTreeItemFlag _flags)
	: m_text(_text), m_iconPath(_iconPath), m_flags(_flags), m_parent(nullptr)
{}

ot::NavigationTreeItem::NavigationTreeItem(const std::string& _text, const std::string& _iconPath, const std::list<NavigationTreeItem>& _childItems, ot::NavigationTreeItemFlag _flags)
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
	// Merge own childs
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

	if (this->text() != _other.text()) return;
	if (this->iconPath() != _other.iconPath()) {
		OT_LOG_WA("Icon path differs on merge. Ignoring");
	}

	// Merge with other
	for (const NavigationTreeItem& item : _other.childItems()) {
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

std::string ot::NavigationTreeItem::itemPath(char _delimiter, const std::string& _suffix) const {
	if (m_parent) {
		return m_parent->itemPath(_delimiter, _delimiter + m_text + _suffix);
	}
	else {
		return m_text + _suffix;
	}
}
