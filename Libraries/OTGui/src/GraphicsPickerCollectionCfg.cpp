// @otlicense

//! @file GraphicsPickerCollectionCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/String.h"
#include "OTCore/LogDispatcher.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/GraphicsPickerCollectionCfg.h"

#define OT_JSON_Member_Name "Name"
#define OT_JSON_Member_Icon "Icon"
#define OT_JSON_Member_Title "Title"
#define OT_JSON_Member_Items "Items"
#define OT_JSON_Member_Collections "Collections"

ot::GraphicsPickerCollectionCfg::GraphicsPickerCollectionCfg() {}

ot::GraphicsPickerCollectionCfg::GraphicsPickerCollectionCfg(const std::string& _collectionName, const std::string& _collectionTitle) 
	: m_name(_collectionName), m_title(_collectionTitle) 
{}

void ot::GraphicsPickerCollectionCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember(OT_JSON_Member_Name, JsonString(m_name, _allocator), _allocator);
	_object.AddMember(OT_JSON_Member_Title, JsonString(m_title, _allocator), _allocator);

	JsonArray collectionArr;
	for (const GraphicsPickerCollectionCfg& c : m_collections) {
		collectionArr.PushBack(JsonObject(c, _allocator), _allocator);
	}
	_object.AddMember(OT_JSON_Member_Collections, collectionArr, _allocator);

	JsonArray itemArr;
	for (const GraphicsPickerItemInfo& i : m_items) {
		itemArr.PushBack(JsonObject(i, _allocator), _allocator);
	}
	_object.AddMember(OT_JSON_Member_Items, itemArr, _allocator);
}

void ot::GraphicsPickerCollectionCfg::setFromJsonObject(const ConstJsonObject& _object) {
	this->clear();

	m_name = json::getString(_object, OT_JSON_Member_Name);
	m_title = json::getString(_object, OT_JSON_Member_Title);

	ConstJsonArray collectionArr = json::getArray(_object, OT_JSON_Member_Collections);
	for (rapidjson::SizeType i = 0; i < collectionArr.Size(); i++) {
		GraphicsPickerCollectionCfg newChild;
		newChild.setFromJsonObject(json::getObject(collectionArr, i));
		m_collections.push_back(std::move(newChild));
	}

	ConstJsonArray itemArr = json::getArray(_object, OT_JSON_Member_Items);
	for (rapidjson::SizeType i = 0; i < itemArr.Size(); i++) {
		GraphicsPickerItemInfo info;
		info.setFromJsonObject(json::getObject(itemArr, i));
		m_items.push_back(std::move(info));
	}
}

ot::GraphicsPickerCollectionCfg& ot::GraphicsPickerCollectionCfg::addChildCollection(const std::string& _collectionName, const std::string& _collectionTitle) {
	GraphicsPickerCollectionCfg newCollection(_collectionName, _collectionTitle);
	return this->addChildCollection(std::move(newCollection));
}

ot::GraphicsPickerCollectionCfg& ot::GraphicsPickerCollectionCfg::addChildCollection(const GraphicsPickerCollectionCfg& _child) {
	GraphicsPickerCollectionCfg newCollection(_child);
	return this->addChildCollection(std::move(newCollection));
}

ot::GraphicsPickerCollectionCfg& ot::GraphicsPickerCollectionCfg::addChildCollection(GraphicsPickerCollectionCfg&& _child) {
	for (GraphicsPickerCollectionCfg& existingChild : m_collections) {
		if (existingChild.getName() == _child.getName()) {
			existingChild.mergeWith(std::move(_child));
			return existingChild;
		}
	}
	m_collections.push_back(std::move(_child));
	return m_collections.back();
}

ot::GraphicsPickerItemInfo& ot::GraphicsPickerCollectionCfg::addItem(const std::string& _itemName, const std::string& _itemTitle, const std::string& _previewIconPath) {
	GraphicsPickerItemInfo newItem(_itemName, _itemTitle, _previewIconPath);
	return this->addItem(std::move(newItem));
}

ot::GraphicsPickerItemInfo& ot::GraphicsPickerCollectionCfg::addItem(const GraphicsPickerItemInfo& _itemInfo) {
	GraphicsPickerItemInfo info(_itemInfo);
	return this->addItem(std::move(info));
}

ot::GraphicsPickerItemInfo& ot::GraphicsPickerCollectionCfg::addItem(GraphicsPickerItemInfo&& _itemInfo) {
	for (GraphicsPickerItemInfo& existingItem : m_items) {
		if (existingItem.getName() == _itemInfo.getName()) {
			OT_LOG_W("Item already exists { \"ItemName\": \"" + existingItem.getName() + "\" }. Ignoring...");
			return existingItem;
		}
	}
	m_items.push_back(std::move(_itemInfo));
	return m_items.back();
}

void ot::GraphicsPickerCollectionCfg::mergeWith(GraphicsPickerCollectionCfg&& _other) {
	// Grab child collections and items from other collection
	std::list<GraphicsPickerCollectionCfg> otherCollections = std::move(_other.m_collections);
	std::list<GraphicsPickerItemInfo> otherItems = std::move(_other.m_items);
	_other.m_collections.clear();
	_other.m_items.clear();

	// Merge child collections
	for (GraphicsPickerCollectionCfg& childCollection : otherCollections) {
		bool found = false;
		for (GraphicsPickerCollectionCfg& existingCollection : m_collections) {
			if (existingCollection.getName() == childCollection.getName()) {
				existingCollection.mergeWith(std::move(childCollection));
				found = true;
				break;
			}
		}

		if (!found) {
			m_collections.push_back(std::move(childCollection));
		}
	}

	// Merge child items
	for (GraphicsPickerItemInfo& itm : otherItems) {
		bool found = false;
		for (GraphicsPickerItemInfo& existingItm : m_items) {
			if (existingItm.getName() == itm.getName()) {
				found = true;
				break;
			}
		}

		if (!found) {
			m_items.push_back(std::move(itm));
		}
	}
}

bool ot::GraphicsPickerCollectionCfg::isEmpty() const {
	bool empty = m_items.empty();
	if (empty) {
		for (const GraphicsPickerCollectionCfg& c : m_collections) {
			if (!c.isEmpty()) {
				empty = false;
				break;
			}
		}
	}
	return empty;
}

void ot::GraphicsPickerCollectionCfg::clear() {
	m_collections.clear();
	m_items.clear();
}
