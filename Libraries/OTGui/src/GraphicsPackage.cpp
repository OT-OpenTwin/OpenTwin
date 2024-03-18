//! @file GraphicsEditorPackage.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################


// OpenTwin header
#include "OTCore/SimpleFactory.h"
#include "OTCore/Logger.h"
#include "OTGui/GraphicsPackage.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/GraphicsPickerCollectionCfg.h"

#define OT_JSON_Member_Name "Name"
#define OT_JSON_Member_Title "Title"
#define OT_JSON_Member_Items "Items"
#define OT_JSON_Member_Collections "Collections"
#define OT_JSON_Member_Connections "Connections"

ot::GraphicsPickerCollectionPackage::GraphicsPickerCollectionPackage() {

}

ot::GraphicsPickerCollectionPackage::~GraphicsPickerCollectionPackage() {
	this->memFree();
}

void ot::GraphicsPickerCollectionPackage::addCollection(GraphicsPickerCollectionCfg* _collection) {
	m_collections.push_back(_collection);
}

void ot::GraphicsPickerCollectionPackage::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	JsonArray collectionArr;
	for (auto c : m_collections) {
		JsonObject collectionObj;
		c->addToJsonObject(collectionObj, _allocator);
		collectionArr.PushBack(collectionObj, _allocator);
	}
	_object.AddMember(OT_JSON_Member_Collections, collectionArr, _allocator);
}

void ot::GraphicsPickerCollectionPackage::setFromJsonObject(const ConstJsonObject& _object) {
	this->memFree();

	std::list<ConstJsonObject> collectionArr = json::getObjectList(_object, OT_JSON_Member_Collections);
	for (auto c : collectionArr) {
		GraphicsPickerCollectionCfg* newCollection = new GraphicsPickerCollectionCfg;
		try {
			newCollection->setFromJsonObject(c);
			m_collections.push_back(newCollection);
		}
		catch (const std::exception& _e) {
			OT_LOG_E(_e.what());
			this->memFree();
			throw _e;
		}
	}
}

void ot::GraphicsPickerCollectionPackage::memFree(void) {
	for (auto c : m_collections) delete c;
	m_collections.clear();
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsNewEditorPackage::GraphicsNewEditorPackage(const std::string& _packageName, const std::string& _editorTitle)
	: m_name(_packageName), m_title(_editorTitle) {}

ot::GraphicsNewEditorPackage::~GraphicsNewEditorPackage() {
	
}

void ot::GraphicsNewEditorPackage::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	ot::GraphicsPickerCollectionPackage::addToJsonObject(_object, _allocator);

	_object.AddMember(OT_JSON_Member_Name, JsonString(m_name, _allocator), _allocator);
	_object.AddMember(OT_JSON_Member_Title, JsonString(m_title, _allocator), _allocator);
}

void ot::GraphicsNewEditorPackage::setFromJsonObject(const ConstJsonObject& _object) {
	ot::GraphicsPickerCollectionPackage::setFromJsonObject(_object);
	m_name = json::getString(_object, OT_JSON_Member_Name);
	m_title = json::getString(_object, OT_JSON_Member_Title);
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsScenePackage::GraphicsScenePackage(const std::string& _editorName) : m_name(_editorName) {

}

ot::GraphicsScenePackage::~GraphicsScenePackage() {
	this->memFree();
}

void ot::GraphicsScenePackage::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember(OT_JSON_Member_Name, JsonString(m_name, _allocator), _allocator);

	JsonArray itemsArr;
	for (auto itm : m_items) {
		JsonObject itemObj;
		itm->addToJsonObject(itemObj, _allocator);
		itemsArr.PushBack(itemObj, _allocator);
	}

	_object.AddMember(OT_JSON_Member_Items, itemsArr, _allocator);
}

void ot::GraphicsScenePackage::setFromJsonObject(const ConstJsonObject& _object) {
	m_name = json::getString(_object, OT_JSON_Member_Name);

	this->memFree();

	std::list<ConstJsonObject> itemsArr = json::getObjectList(_object, OT_JSON_Member_Items);
	for (auto itmObj : itemsArr) {
		ot::GraphicsItemCfg* itm = ot::SimpleFactory::instance().createType<ot::GraphicsItemCfg>(itmObj);
		OTAssertNullptr(itm);
		if (itm) {
			itm->setFromJsonObject(itmObj);
			if (itm) m_items.push_back(itm);
		}
	}
}

void ot::GraphicsScenePackage::addItem(GraphicsItemCfg* _item) {
	OTAssertNullptr(_item);
	m_items.push_back(_item);
}

void ot::GraphicsScenePackage::memFree(void) {
	for (auto itm : m_items) delete itm;
	m_items.clear();
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsConnectionPackage::GraphicsConnectionPackage(const std::string& _editorName) : m_name(_editorName) {

}

ot::GraphicsConnectionPackage::~GraphicsConnectionPackage() {

}

void ot::GraphicsConnectionPackage::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	JsonArray cArr;
	for (auto c : m_connections) {
		JsonObject cObj;
		c.addToJsonObject(cObj, _allocator);
		cArr.PushBack(cObj, _allocator);
	}
	_object.AddMember(OT_JSON_Member_Connections, cArr, _allocator);
	_object.AddMember(OT_JSON_Member_Name, JsonString(m_name, _allocator), _allocator);
}

void ot::GraphicsConnectionPackage::setFromJsonObject(const ConstJsonObject& _object) {
	m_connections.clear();

	m_name = json::getString(_object, OT_JSON_Member_Name);

	std::list<ConstJsonObject> cArr = json::getObjectList(_object, OT_JSON_Member_Connections);
	for (auto c : cArr) {
		GraphicsConnectionCfg newConnection;
		newConnection.setFromJsonObject(c);

		m_connections.push_back(newConnection);
	}
}

void ot::GraphicsConnectionPackage::addConnection(const ot::UID& _fromUid, const std::string& _fromConnectable, const ot::UID& _toUid, const std::string& _toConnectable) {
	GraphicsConnectionCfg newConnection(_fromUid, _fromConnectable, _toUid, _toConnectable);
	this->addConnection(newConnection);
}
