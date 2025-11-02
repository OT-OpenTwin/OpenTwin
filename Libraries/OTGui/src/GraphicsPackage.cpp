// @otlicense
// File: GraphicsPackage.cpp
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
#include "OTGui/GraphicsPackage.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/GraphicsItemCfgFactory.h"
#include "OTGui/GraphicsPickerCollectionCfg.h"

void ot::GraphicsPickerCollectionPackage::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	JsonArray collectionArr;
	for (auto& c : m_collections) {
		collectionArr.PushBack(JsonObject(c, _allocator), _allocator);
	}
	_object.AddMember("Collections", collectionArr, _allocator);
	_object.AddMember("PickerKey", JsonString(m_pickerKey, _allocator), _allocator);

	if (m_pickerKey.empty()) {
		OT_LOG_WA("Picker key is empty");
	}
}

void ot::GraphicsPickerCollectionPackage::setFromJsonObject(const ConstJsonObject& _object) {
	m_pickerKey = json::getString(_object, "PickerKey");
	std::list<ConstJsonObject> collectionArr = json::getObjectList(_object, "Collections");
	for (const ConstJsonObject& c : collectionArr) {
		GraphicsPickerCollectionCfg newCollection;
		newCollection.setFromJsonObject(c);
		m_collections.push_back(std::move(newCollection));
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::GraphicsNewEditorPackage::GraphicsNewEditorPackage(const std::string& _packageName, const std::string& _editorTitle)
	: m_name(_packageName), m_title(_editorTitle) {}

void ot::GraphicsNewEditorPackage::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	ot::GraphicsPickerCollectionPackage::addToJsonObject(_object, _allocator);

	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Title", JsonString(m_title, _allocator), _allocator);
}

void ot::GraphicsNewEditorPackage::setFromJsonObject(const ConstJsonObject& _object) {
	ot::GraphicsPickerCollectionPackage::setFromJsonObject(_object);
	m_name = json::getString(_object, "Name");
	m_title = json::getString(_object, "Title");
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
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);

	JsonArray itemsArr;
	for (auto itm : m_items) {
		JsonObject itemObj;
		itm->addToJsonObject(itemObj, _allocator);
		itemsArr.PushBack(itemObj, _allocator);
	}

	_object.AddMember("Items", itemsArr, _allocator);
	_object.AddMember("PickerKey", JsonString(m_pickerKey, _allocator), _allocator);
}

void ot::GraphicsScenePackage::setFromJsonObject(const ConstJsonObject& _object) {
	m_name = json::getString(_object, "Name");
	m_pickerKey = json::getString(_object, "PickerKey");

	this->memFree();

	std::list<ConstJsonObject> itemsArr = json::getObjectList(_object, "Items");
	for (auto itmObj : itemsArr) {
		ot::GraphicsItemCfg* itm = GraphicsItemCfgFactory::create(itmObj);
		OTAssertNullptr(itm);
		if (itm) m_items.push_back(itm);
	}
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

void ot::GraphicsConnectionPackage::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	JsonArray cArr;
	for (auto c : m_connections) {
		JsonObject cObj;
		c.addToJsonObject(cObj, _allocator);
		cArr.PushBack(cObj, _allocator);
	}
	_object.AddMember("Connections", cArr, _allocator);
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("PickerKey", JsonString(m_pickerKey, _allocator), _allocator);
}

void ot::GraphicsConnectionPackage::setFromJsonObject(const ConstJsonObject& _object) {
	m_connections.clear();

	m_name = json::getString(_object, "Name");
	m_pickerKey = json::getString(_object, "PickerKey");

	std::list<ConstJsonObject> cArr = json::getObjectList(_object, "Connections");
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
