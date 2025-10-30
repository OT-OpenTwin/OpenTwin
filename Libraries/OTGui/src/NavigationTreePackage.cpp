// @otlicense

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/LogDispatcher.h"
#include "OTGui/NavigationTreePackage.h"

std::string ot::NavigationTreePackage::toString(NavigationTreePackageFlag _flag) {
	switch (_flag)
	{
	case NoFlags: return "None";
	case ItemsDefaultExpanded: return "ItemsDefaultExpanded";
	default:
		OT_LOG_WAS("Unknown flag: " + std::to_string((int)_flag));
		return "None";
	}
}

ot::NavigationTreePackage::NavigationTreePackageFlag ot::NavigationTreePackage::stringToFlag(const std::string& _flag) {
	if (_flag == toString(NoFlags)) return NoFlags;
	else if (_flag == toString(ItemsDefaultExpanded)) return ItemsDefaultExpanded;
	else {
		OT_LOG_WAS("Unknown flag: \"" + _flag + "\"");
		return NoFlags;
	}
}

ot::NavigationTreePackage::NavigationTreePackage() : m_flags(NoFlags) {}

ot::NavigationTreePackage::~NavigationTreePackage() {}

void ot::NavigationTreePackage::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	JsonArray flagArr;
	if (m_flags & ItemsDefaultExpanded) flagArr.PushBack(JsonString(toString(ItemsDefaultExpanded), _allocator), _allocator);
	_object.AddMember("Flags", flagArr, _allocator);

	JsonArray rootArr;
	for (const NavigationTreeItem& itm : m_rootItems) {
		JsonObject itmObj;
		itm.addToJsonObject(itmObj, _allocator);
		rootArr.PushBack(itmObj, _allocator);
	}
	_object.AddMember("RootItems", rootArr, _allocator);
}

void ot::NavigationTreePackage::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_flags = NoFlags;
	ConstJsonArray flagArr = json::getArray(_object, "Flags");
	for (JsonSizeType i = 0; i < flagArr.Size(); i++) {
		m_flags |= stringToFlag(json::getString(flagArr, i, toString(NoFlags)));
	}

	m_rootItems.clear();
	ConstJsonArray rootArr = json::getArray(_object, "RootItems");
	for (JsonSizeType i = 0; i < rootArr.Size(); i++) {
		ConstJsonObject itmObj = json::getObject(rootArr, i);
		NavigationTreeItem itm;
		itm.setFromJsonObject(itmObj);
		m_rootItems.push_back(itm);
	}
}

void ot::NavigationTreePackage::addRootItem(const NavigationTreeItem& _item) {
	m_rootItems.push_back(_item);
}

void ot::NavigationTreePackage::mergeItems(bool _mergeAllChilds) {
	std::list<NavigationTreeItem> bck = m_rootItems;
	m_rootItems.clear();
	bool exists = false;
	for (const NavigationTreeItem& item : bck) {
		exists = false;
		for (NavigationTreeItem& eItem : m_rootItems) {
			if (eItem.getText() == item.getText()) {
				eItem.merge(item);
				exists = true;
				break;
			}
		}
		if (!exists) m_rootItems.push_back(item);
	}
}