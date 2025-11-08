// @otlicense
// File: GraphicsActionHandler.cpp
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

ot::JsonDocument ot::GraphicsActionHandler::createItemRequestedDocument(const GraphicsItemDropEvent& _eventData) {
	JsonDocument doc;
	
	doc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddItem, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Event, JsonObject(_eventData, doc.GetAllocator()), doc.GetAllocator());

	return doc;
}

ot::JsonDocument ot::GraphicsActionHandler::createItemDoubleClickedDocument(const GraphicsDoubleClickEvent& _eventData) {
	JsonDocument doc;

	doc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_ItemDoubleClicked, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Event, JsonObject(_eventData, doc.GetAllocator()), doc.GetAllocator());

	return doc;
}

ot::JsonDocument ot::GraphicsActionHandler::createConnectionRequestedDocument(const GraphicsConnectionDropEvent& _eventData) {
	JsonDocument doc;

	doc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnection, doc.GetAllocator()), doc.GetAllocator());
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
	m_actionHandler.connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_ItemDoubleClicked, this, &GraphicsActionHandler::handleGraphicsItemDoubleClicked, ot::SECURE_MESSAGE_TYPES);
	
	m_actionHandler.connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnection, this, &GraphicsActionHandler::handleGraphicsConnectionRequested, ot::SECURE_MESSAGE_TYPES);

	m_actionHandler.connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_ChangeEvent, this, &GraphicsActionHandler::handleGraphicsChangeEvent, ot::SECURE_MESSAGE_TYPES);

}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Handlers

ot::ReturnMessage ot::GraphicsActionHandler::handleGraphicsItemRequested(JsonDocument& _document) {
	GraphicsItemDropEvent eventData(json::getObject(_document, OT_ACTION_PARAM_Event));

	return graphicsItemRequested(eventData);
}

ot::ReturnMessage ot::GraphicsActionHandler::handleGraphicsItemDoubleClicked(JsonDocument& _document) {
	GraphicsDoubleClickEvent eventData(json::getObject(_document, OT_ACTION_PARAM_Event));

	return graphicsItemDoubleClicked(eventData);
}

ot::ReturnMessage ot::GraphicsActionHandler::handleGraphicsConnectionRequested(JsonDocument& _document) {
	GraphicsConnectionDropEvent eventData(json::getObject(_document, OT_ACTION_PARAM_Event));

	return graphicsConnectionRequested(eventData);
}

ot::ReturnMessage ot::GraphicsActionHandler::handleGraphicsChangeEvent(JsonDocument& _document) {
	GraphicsChangeEvent event(json::getObject(_document, OT_ACTION_PARAM_Event));
	return graphicsChangeEvent(event);
}
