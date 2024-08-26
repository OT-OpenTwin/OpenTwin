//! @file GraphicsPickerCollectionCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
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

ot::GraphicsPickerCollectionCfg::GraphicsPickerCollectionCfg(const GraphicsPickerCollectionCfg& _other) {
	*this = _other;
}

ot::GraphicsPickerCollectionCfg::~GraphicsPickerCollectionCfg() {
	this->memFree();
}

ot::GraphicsPickerCollectionCfg& ot::GraphicsPickerCollectionCfg::operator=(const GraphicsPickerCollectionCfg& _other) {
	if (this == &_other) return *this;
	this->memFree();

	m_name = _other.m_name;
	m_title = _other.m_title;
	
	m_items = _other.m_items;

	for (const GraphicsPickerCollectionCfg* child : _other.m_collections) {
		m_collections.push_back(new GraphicsPickerCollectionCfg(*child));
	}

	return *this;
}

void ot::GraphicsPickerCollectionCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember(OT_JSON_Member_Name, JsonString(m_name, _allocator), _allocator);
	_object.AddMember(OT_JSON_Member_Title, JsonString(m_title, _allocator), _allocator);

	JsonArray collectionArr;
	for (auto c : m_collections) {
		JsonObject collectionObj;
		c->addToJsonObject(collectionObj, _allocator);
		collectionArr.PushBack(collectionObj, _allocator);
	}
	_object.AddMember(OT_JSON_Member_Collections, collectionArr, _allocator);

	JsonArray itemArr;
	for (auto i : m_items) {
		JsonObject infoObj;
		i.addToJsonObject(infoObj, _allocator);
		itemArr.PushBack(infoObj, _allocator);
	}
	_object.AddMember(OT_JSON_Member_Items, itemArr, _allocator);
}

void ot::GraphicsPickerCollectionCfg::setFromJsonObject(const ConstJsonObject& _object) {
	this->memFree();

	m_name = json::getString(_object, OT_JSON_Member_Name);
	m_title = json::getString(_object, OT_JSON_Member_Title);

	ConstJsonArray collectionArr = json::getArray(_object, OT_JSON_Member_Collections);
	for (rapidjson::SizeType i = 0; i < collectionArr.Size(); i++) {
		ConstJsonObject collectionObj = json::getObject(collectionArr, i);
		GraphicsPickerCollectionCfg* newChild = new GraphicsPickerCollectionCfg;
		try {
			newChild->setFromJsonObject(collectionObj);
			m_collections.push_back(newChild);
		}
		catch (...) {
			OT_LOG_E("Failed to create child collection. Abort");
			delete newChild;
			this->memFree();
			throw std::exception("Failed to create graphics collection");
		}
	}

	ConstJsonArray itemArr = json::getArray(_object, OT_JSON_Member_Items);
	for (rapidjson::SizeType i = 0; i < itemArr.Size(); i++) {
		ConstJsonObject infoObj = json::getObject(itemArr, i);
		GraphicsPickerItemInformation info;
		info.setFromJsonObject(infoObj);
		m_items.push_back(info);
	}
}

void ot::GraphicsPickerCollectionCfg::addChildCollection(GraphicsPickerCollectionCfg* _child) {
	OTAssertNullptr(_child);
	m_collections.push_back(_child);
}

void ot::GraphicsPickerCollectionCfg::addItem(const std::string& _itemName, const std::string& _itemTitle, const std::string& _previewIconPath) {
	this->addItem(GraphicsPickerItemInformation(_itemName, _itemTitle, _previewIconPath));
}

void ot::GraphicsPickerCollectionCfg::addItem(const GraphicsPickerItemInformation& _itemInfo) {
	m_items.push_back(_itemInfo);
}

void ot::GraphicsPickerCollectionCfg::mergeWith(const GraphicsPickerCollectionCfg& _other) {
	// Merge child collections
	for (const GraphicsPickerCollectionCfg* childCollection : _other.getChildCollections()) {
		bool found = false;
		for (GraphicsPickerCollectionCfg* existingCollection : m_collections) {
			if (existingCollection->getName() == childCollection->getName()) {
				existingCollection->mergeWith(*childCollection);
				found = true;
				break;
			}
		}

		if (!found) {
			m_collections.push_back(new GraphicsPickerCollectionCfg(*childCollection));
		}
	}

	// Merge child items
	for (const GraphicsPickerItemInformation& itm : _other.getItems()) {
		bool found = false;
		for (const GraphicsPickerItemInformation& existingItm : m_items) {
			if (existingItm.getName() == itm.getName()) {
				found = true;
				break;
			}
		}

		if (!found) {
			m_items.push_back(itm);
		}
	}
}

void ot::GraphicsPickerCollectionCfg::memFree(void) {
	for (auto c : m_collections) delete c;
	m_collections.clear();

	m_items.clear();
}
