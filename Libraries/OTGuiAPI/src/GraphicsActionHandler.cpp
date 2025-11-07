// @otlicense
// File: GraphicsHandler.cpp
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
#include "OTGui/GraphicsItemCfgFactory.h"
#include "OTGuiAPI/GraphicsActionHandler.h"

ot::JsonDocument ot::GraphicsActionHandler::createItemRequestedDocument(const std::string& _viewName, const std::string& _itemName, const ot::Point2DD& _pos, const GuiEvent& _eventData) {
	JsonDocument doc;
	
	doc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddItem, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName, JsonString(_viewName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_ItemName, JsonString(_itemName, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_ItemPosition, JsonObject(_pos, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Event, JsonObject(_eventData, doc.GetAllocator()), doc.GetAllocator());
	return doc;
}

ot::JsonDocument ot::GraphicsActionHandler::createItemChangedDocument(const ot::GraphicsItemCfg* _item, const GuiEvent& _eventData) {
	JsonDocument doc;

	doc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_ItemChanged, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Config, JsonObject(_item, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Event, JsonObject(_eventData, doc.GetAllocator()), doc.GetAllocator());

	return doc;
}

ot::JsonDocument ot::GraphicsActionHandler::createItemDoubleClickedDocument(const std::string& _name, ot::UID _uid, const GuiEvent& _eventData) {
	JsonDocument doc;

	doc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_ItemDoubleClicked, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_NAME, JsonString(_name, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UID, _uid, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Event, JsonObject(_eventData, doc.GetAllocator()), doc.GetAllocator());

	return doc;
}

ot::JsonDocument ot::GraphicsActionHandler::createConnectionRequestedDocument(const ot::GraphicsConnectionPackage& _connectionData, const GuiEvent& _eventData) {
	JsonDocument doc;

	doc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnection, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, JsonObject(_connectionData, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Event, JsonObject(_eventData, doc.GetAllocator()), doc.GetAllocator());

	return doc;
}

ot::JsonDocument ot::GraphicsActionHandler::createConnectionToConnectionRequestedDocument(const ot::GraphicsConnectionPackage& _connectionData, const Point2DD& _pos, const GuiEvent& _eventData) {
	JsonDocument doc;

	doc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnectionToConnection, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, JsonObject(_connectionData, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_POSITION, JsonObject(_pos, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Event, JsonObject(_eventData, doc.GetAllocator()), doc.GetAllocator());

	return doc;
}

ot::JsonDocument ot::GraphicsActionHandler::createConnectionChangedDocument(const ot::GraphicsConnectionCfg& _connectionData, const GuiEvent& _eventData) {
	JsonDocument doc;

	doc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_ConnectionChanged, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Config, JsonObject(_connectionData, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Event, JsonObject(_eventData, doc.GetAllocator()), doc.GetAllocator());

	return doc;
}

ot::JsonDocument ot::GraphicsActionHandler::createChangeEventDocument(const ot::GraphicsChangeEvent& _changeEvent) {
	JsonDocument doc;

	doc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_ChangeEvent, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Event, JsonObject(_changeEvent, doc.GetAllocator()), doc.GetAllocator());

	return doc;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Constructor

ot::GraphicsActionHandler::GraphicsActionHandler(ActionDispatcherBase* _dispatcher)
	: m_actionHandler(_dispatcher) 
{
	m_actionHandler.connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddItem, this, &GraphicsActionHandler::handleGraphicsItemRequested, ot::SECURE_MESSAGE_TYPES);
	m_actionHandler.connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_ItemChanged, this, &GraphicsActionHandler::handleGraphicsItemChanged, ot::SECURE_MESSAGE_TYPES);
	m_actionHandler.connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_ItemDoubleClicked, this, &GraphicsActionHandler::handleGraphicsItemDoubleClicked, ot::SECURE_MESSAGE_TYPES);
	
	m_actionHandler.connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnection, this, &GraphicsActionHandler::handleGraphicsConnectionRequested, ot::SECURE_MESSAGE_TYPES);
	m_actionHandler.connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnectionToConnection, this, &GraphicsActionHandler::handleGraphicsConnectionToConnectionRequested, ot::SECURE_MESSAGE_TYPES);
	m_actionHandler.connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_ConnectionChanged, this, &GraphicsActionHandler::handleGraphicsConnectionChanged, ot::SECURE_MESSAGE_TYPES);

	m_actionHandler.connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_ChangeEvent, this, &GraphicsActionHandler::handleGraphicsChangeEvent, ot::SECURE_MESSAGE_TYPES);

}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Handlers

ot::ReturnMessage ot::GraphicsActionHandler::handleGraphicsItemRequested(JsonDocument& _document) {
	std::string itemName = json::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemName);
	std::string editorName = json::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName);
	GuiEvent eventData(json::getObject(_document, OT_ACTION_PARAM_Event));

	ot::Point2DD pos;
	pos.setFromJsonObject(json::getObject(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemPosition));

	return graphicsItemRequested(editorName, itemName, pos, eventData);
}

ot::ReturnMessage ot::GraphicsActionHandler::handleGraphicsItemChanged(JsonDocument& _document) {
	std::unique_ptr<ot::GraphicsItemCfg> itemConfig(GraphicsItemCfgFactory::instance().create(json::getObject(_document, OT_ACTION_PARAM_Config)));
	if (!itemConfig.get()) {
		return ot::ReturnMessage(ReturnMessage::Failed, "Failed to create graphics item configuration from received data");
	}

	GuiEvent eventData(json::getObject(_document, OT_ACTION_PARAM_Event));

	return graphicsItemChanged(itemConfig.get(), eventData);
}

ot::ReturnMessage ot::GraphicsActionHandler::handleGraphicsItemDoubleClicked(JsonDocument& _document) {
	std::string name = json::getString(_document, OT_ACTION_PARAM_NAME);
	UID uid = json::getUInt64(_document, OT_ACTION_PARAM_UID);
	GuiEvent eventData(json::getObject(_document, OT_ACTION_PARAM_Event));

	return graphicsItemDoubleClicked(name, uid, eventData);
}

ot::ReturnMessage ot::GraphicsActionHandler::handleGraphicsConnectionRequested(JsonDocument& _document) {
	GraphicsConnectionPackage pckg;
	pckg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));
	GuiEvent eventData(json::getObject(_document, OT_ACTION_PARAM_Event));

	return graphicsConnectionRequested(pckg, eventData);
}

ot::ReturnMessage ot::GraphicsActionHandler::handleGraphicsConnectionToConnectionRequested(JsonDocument& _document) {
	GraphicsConnectionPackage pckg;
	pckg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));

	Point2DD pos;
	pos.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_POSITION));

	GuiEvent eventData(json::getObject(_document, OT_ACTION_PARAM_Event));

	return graphicsConnectionToConnectionRequested(pckg, pos, eventData);
}

ot::ReturnMessage ot::GraphicsActionHandler::handleGraphicsConnectionChanged(JsonDocument& _document) {
	GraphicsConnectionCfg cfg;
	cfg.setFromJsonObject(json::getObject(_document, OT_ACTION_PARAM_Config));
	GuiEvent eventData(json::getObject(_document, OT_ACTION_PARAM_Event));

	return graphicsConnectionChanged(cfg, eventData);
}

ot::ReturnMessage ot::GraphicsActionHandler::handleGraphicsChangeEvent(JsonDocument& _document) {
	GraphicsChangeEvent event(json::getObject(_document, OT_ACTION_PARAM_Event));
	return graphicsChangeEvent(event);
}
