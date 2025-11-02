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

ot::GraphicsActionHandler::GraphicsActionHandler(ActionDispatcherBase* _dispatcher)
	: m_actionHandler(_dispatcher) 
{
	m_actionHandler.connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddItem, this, &GraphicsActionHandler::handleGraphicsItemRequested, ot::SECURE_MESSAGE_TYPES);
	m_actionHandler.connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_ItemChanged, this, &GraphicsActionHandler::handleGraphicsItemChanged, ot::SECURE_MESSAGE_TYPES);
	m_actionHandler.connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_ItemDoubleClicked, this, &GraphicsActionHandler::handleGraphicsItemDoubleClicked, ot::SECURE_MESSAGE_TYPES);
	
	m_actionHandler.connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_RemoveItem, this, &GraphicsActionHandler::handleGraphicsRemoveRequested, ot::SECURE_MESSAGE_TYPES);
	
	m_actionHandler.connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnection, this, &GraphicsActionHandler::handleGraphicsConnectionRequested, ot::SECURE_MESSAGE_TYPES);
	m_actionHandler.connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnectionToConnection, this, &GraphicsActionHandler::handleGraphicsConnectionToConnectionRequested, ot::SECURE_MESSAGE_TYPES);
	m_actionHandler.connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_ConnectionChanged, this, &GraphicsActionHandler::handleGraphicsConnectionChanged, ot::SECURE_MESSAGE_TYPES);

	m_actionHandler.connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_ChangeEvent, this, &GraphicsActionHandler::handleGraphicsChangeEvent, ot::SECURE_MESSAGE_TYPES);

}

ot::ReturnMessage ot::GraphicsActionHandler::handleGraphicsItemRequested(JsonDocument& _document) {
	std::string itemName = json::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemName);
	std::string editorName = json::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName);

	ot::Point2DD pos;
	pos.setFromJsonObject(json::getObject(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemPosition));

	return graphicsItemRequested(editorName, itemName, pos);
}

ot::ReturnMessage ot::GraphicsActionHandler::handleGraphicsItemChanged(JsonDocument& _document) {
	std::unique_ptr<ot::GraphicsItemCfg> itemConfig(GraphicsItemCfgFactory::instance().create(json::getObject(_document, OT_ACTION_PARAM_Config)));
	if (!itemConfig.get()) {
		return ot::ReturnMessage(ReturnMessage::Failed, "Failed to create graphics item configuration from received data");
	}

	return graphicsItemChanged(itemConfig.get());
}

ot::ReturnMessage ot::GraphicsActionHandler::handleGraphicsItemDoubleClicked(JsonDocument& _document) {
	std::string name = json::getString(_document, OT_ACTION_PARAM_NAME);
	UID uid = json::getUInt64(_document, OT_ACTION_PARAM_UID);

	return graphicsItemDoubleClicked(name, uid);
}

ot::ReturnMessage ot::GraphicsActionHandler::handleGraphicsRemoveRequested(JsonDocument& _document) {
	ot::UIDList items = json::getUInt64List(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemIds);
	ot::UIDList connections = json::getUInt64List(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_ConnectionIds);
	
	return graphicsRemoveRequested(items, connections);
}

ot::ReturnMessage ot::GraphicsActionHandler::handleGraphicsConnectionRequested(JsonDocument& _document) {
	GraphicsConnectionPackage pckg;
	pckg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));

	return graphicsConnectionRequested(pckg);
}

ot::ReturnMessage ot::GraphicsActionHandler::handleGraphicsConnectionToConnectionRequested(JsonDocument& _document) {
	GraphicsConnectionPackage pckg;
	pckg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));

	Point2DD pos;
	pos.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_POSITION));

	return graphicsConnectionToConnectionRequested(pckg, pos);
}

ot::ReturnMessage ot::GraphicsActionHandler::handleGraphicsConnectionChanged(JsonDocument& _document) {
	GraphicsConnectionCfg cfg;
	cfg.setFromJsonObject(json::getObject(_document, OT_ACTION_PARAM_Config));

	return graphicsConnectionChanged(cfg);
}

ot::ReturnMessage ot::GraphicsActionHandler::handleGraphicsChangeEvent(JsonDocument& _document) {
	GraphicsChangeEvent event(json::getObject(_document, OT_ACTION_PARAM_Event));
	return graphicsChangeEvent(event);
}
