// @otlicense
// File: GraphicsChangeEvent.cpp
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
#include "OTGui/GraphicsChangeEvent.h"
#include "OTGui/GraphicsItemCfgFactory.h"

ot::GraphicsChangeEvent::GraphicsChangeEvent(const ConstJsonObject& _jsonObject) : GraphicsChangeEvent() {
	setFromJsonObject(_jsonObject);
}

ot::GraphicsChangeEvent::GraphicsChangeEvent(const GraphicsChangeEvent& _other) : GraphicsChangeEvent() {
	operator=(_other);
}

ot::GraphicsChangeEvent::GraphicsChangeEvent(GraphicsChangeEvent&& _other) noexcept 
	: GuiEvent(std::move(_other))
{
	m_editorName = std::move(_other.m_editorName);
	m_changedItems = std::move(_other.m_changedItems);
	m_changedConnections = std::move(_other.m_changedConnections);

	_other.m_changedItems.clear();
	_other.m_changedConnections.clear();
}

ot::GraphicsChangeEvent::~GraphicsChangeEvent() {
	memFree();
}

ot::GraphicsChangeEvent& ot::GraphicsChangeEvent::operator=(const GraphicsChangeEvent& _other) {
	if (this != &_other) {
		memFree();

		GuiEvent::operator=(_other);

		m_editorName = _other.m_editorName;

		for (const GraphicsItemCfg* item : _other.m_changedItems) {
			m_changedItems.push_back(item->createCopy());
		}

		m_changedConnections = _other.m_changedConnections;
	}

	return *this;
}

ot::GraphicsChangeEvent& ot::GraphicsChangeEvent::operator=(GraphicsChangeEvent&& _other) noexcept {
	if (this != &_other) {
		memFree();

		GuiEvent::operator=(std::move(_other));

		m_editorName = std::move(_other.m_editorName);
		m_changedItems = std::move(_other.m_changedItems);
		m_changedConnections = std::move(_other.m_changedConnections);
		
		_other.m_changedItems.clear();
		_other.m_changedConnections.clear();
	}

	return *this;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Virtual methods

void ot::GraphicsChangeEvent::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	GuiEvent::addToJsonObject(_object, _allocator);

	_object.AddMember("EditorName", JsonString(m_editorName, _allocator), _allocator);
	
	JsonArray itemsArr;
	for (const GraphicsItemCfg* item : m_changedItems) {
		itemsArr.PushBack(JsonObject(item, _allocator), _allocator);
	}
	_object.AddMember("ChangedItems", itemsArr, _allocator);

	JsonArray connectionsArr;
	for (const GraphicsConnectionCfg& conn : m_changedConnections) {
		connectionsArr.PushBack(JsonObject(conn, _allocator), _allocator);
	}
	_object.AddMember("ChangedConnections", connectionsArr, _allocator);
}

void ot::GraphicsChangeEvent::setFromJsonObject(const ConstJsonObject& _object) {
	memFree();

	GuiEvent::setFromJsonObject(_object);
	
	m_editorName = json::getString(_object, "EditorName");
	
	for (const ConstJsonObject& itemObj : json::getObjectList(_object, "ChangedItems")) {
		GraphicsItemCfg* item = GraphicsItemCfgFactory::create(itemObj);
		if (item) {
			m_changedItems.push_back(item);
		}
		else {
			OT_LOG_EA("Failed to deserialize GraphicsItem");
		}
	}

	for (const ConstJsonObject& connObj : json::getObjectList(_object, "ChangedConnections")) {
		m_changedConnections.push_back(GraphicsConnectionCfg(connObj));
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

void ot::GraphicsChangeEvent::memFree() {
	for (GraphicsItemCfg* item : m_changedItems) {
		OTAssertNullptr(item);
		delete item;
	}
	m_changedItems.clear();
	m_changedConnections.clear();
	m_editorName.clear();
}