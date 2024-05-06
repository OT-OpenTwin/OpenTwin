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

ot::GraphicsPickerCollectionCfg::GraphicsPickerCollectionCfg(const std::string& _collectionName, const std::string& _collectionTitle) : m_name(_collectionName), m_title(_collectionTitle) {

}

ot::GraphicsPickerCollectionCfg::~GraphicsPickerCollectionCfg() {
	this->memFree();
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
		infoObj.AddMember(OT_JSON_Member_Name, JsonString(i.itemName, _allocator), _allocator);
		infoObj.AddMember(OT_JSON_Member_Title, JsonString(i.itemTitle, _allocator), _allocator);
		infoObj.AddMember(OT_JSON_Member_Icon, JsonString(i.previewIcon, _allocator), _allocator);
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
		
		ItemInformation info;
		info.itemName = json::getString(infoObj, OT_JSON_Member_Name);
		info.itemTitle = json::getString(infoObj, OT_JSON_Member_Title);
		info.previewIcon = json::getString(infoObj, OT_JSON_Member_Icon);
		m_items.push_back(info);
	}
}

void ot::GraphicsPickerCollectionCfg::addChildCollection(GraphicsPickerCollectionCfg* _child) {
	OTAssertNullptr(_child);
	m_collections.push_back(_child);
}

void ot::GraphicsPickerCollectionCfg::addItem(const std::string& _itemName, const std::string& _itemTitle, const std::string& _previewIconPath) {
	ItemInformation info;
	info.itemName = _itemName;
	info.itemTitle = _itemTitle;
	info.previewIcon = _previewIconPath;
	m_items.push_back(info);
}

void ot::GraphicsPickerCollectionCfg::memFree(void) {
	for (auto c : m_collections) delete c;
	m_collections.clear();

	m_items.clear();
}
