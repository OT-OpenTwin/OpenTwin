//! @file GraphicsHandler.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsItemCfgFactory.h"
#include "OTGuiAPI/GraphicsHandler.h"

ot::GraphicsHandler::GraphicsHandler(ActionDispatcherBase* _dispatcher)
	: m_actionHandler(_dispatcher) 
{
	m_actionHandler.connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddItem, this, &GraphicsHandler::handleGraphicsItemRequested, ot::SECURE_MESSAGE_TYPES);
	m_actionHandler.connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_ItemChanged, this, &GraphicsHandler::handleGraphicsItemChanged, ot::SECURE_MESSAGE_TYPES);
	m_actionHandler.connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_ItemDoubleClicked, this, &GraphicsHandler::handleGraphicsItemDoubleClicked, ot::SECURE_MESSAGE_TYPES);
	
	m_actionHandler.connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_RemoveItem, this, &GraphicsHandler::handleGraphicsRemoveRequested, ot::SECURE_MESSAGE_TYPES);
	
	m_actionHandler.connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnection, this, &GraphicsHandler::handleGraphicsConnectionRequested, ot::SECURE_MESSAGE_TYPES);
	m_actionHandler.connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnectionToConnection, this, &GraphicsHandler::handleGraphicsConnectionToConnectionRequested, ot::SECURE_MESSAGE_TYPES);
	m_actionHandler.connectAction(OT_ACTION_CMD_UI_GRAPHICSEDITOR_ConnectionChanged, this, &GraphicsHandler::handleGraphicsConnectionChanged, ot::SECURE_MESSAGE_TYPES);

}

ot::ReturnMessage ot::GraphicsHandler::graphicsItemRequested(const std::string& _viewName, const std::string& _itemName, const ot::Point2DD& _pos) { return ReturnMessage(ReturnMessage::Ok, "Request ignored"); }
ot::ReturnMessage ot::GraphicsHandler::graphicsItemChanged(const ot::GraphicsItemCfg* _item) { return ReturnMessage(ReturnMessage::Ok, "Request ignored"); }
ot::ReturnMessage ot::GraphicsHandler::graphicsItemDoubleClicked(const std::string& _name, UID _uid) { return ReturnMessage(ReturnMessage::Ok, "Request ignored"); }
ot::ReturnMessage ot::GraphicsHandler::graphicsRemoveRequested(const UIDList& _itemUids, const UIDList& _connectionUids) { return ReturnMessage(ReturnMessage::Ok, "Request ignored"); }
ot::ReturnMessage ot::GraphicsHandler::graphicsConnectionRequested(const ot::GraphicsConnectionPackage& _connectionData) { return ReturnMessage(ReturnMessage::Ok, "Request ignored"); }
ot::ReturnMessage ot::GraphicsHandler::graphicsConnectionToConnectionRequested(const ot::GraphicsConnectionPackage& _connectionData, const ot::Point2DD& _pos) { return ReturnMessage(ReturnMessage::Ok, "Request ignored"); }
ot::ReturnMessage ot::GraphicsHandler::graphicsConnectionChanged(const ot::GraphicsConnectionCfg& _connectionData) { return ReturnMessage(ReturnMessage::Ok, "Request ignored"); }

ot::ReturnMessage ot::GraphicsHandler::handleGraphicsItemRequested(JsonDocument& _document) {
	std::string itemName = json::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemName);
	std::string editorName = json::getString(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_EditorName);

	ot::Point2DD pos;
	pos.setFromJsonObject(json::getObject(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemPosition));

	return graphicsItemRequested(editorName, itemName, pos);
}

ot::ReturnMessage ot::GraphicsHandler::handleGraphicsItemChanged(JsonDocument& _document) {
	std::unique_ptr<ot::GraphicsItemCfg> itemConfig(GraphicsItemCfgFactory::instance().create(json::getObject(_document, OT_ACTION_PARAM_Config)));
	if (!itemConfig.get()) {
		return ot::ReturnMessage(ReturnMessage::Failed, "Failed to create graphics item configuration from received data");
	}

	return graphicsItemChanged(itemConfig.get());
}

ot::ReturnMessage ot::GraphicsHandler::handleGraphicsItemDoubleClicked(JsonDocument& _document) {
	std::string name = json::getString(_document, OT_ACTION_PARAM_NAME);
	UID uid = json::getUInt64(_document, OT_ACTION_PARAM_UID);

	return graphicsItemDoubleClicked(name, uid);
}

ot::ReturnMessage ot::GraphicsHandler::handleGraphicsRemoveRequested(JsonDocument& _document) {
	ot::UIDList items = json::getUInt64List(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_ItemIds);
	ot::UIDList connections = json::getUInt64List(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_ConnectionIds);
	
	return graphicsRemoveRequested(items, connections);
}

ot::ReturnMessage ot::GraphicsHandler::handleGraphicsConnectionRequested(JsonDocument& _document) {
	GraphicsConnectionPackage pckg;
	pckg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));

	return graphicsConnectionRequested(pckg);
}

ot::ReturnMessage ot::GraphicsHandler::handleGraphicsConnectionToConnectionRequested(JsonDocument& _document) {
	GraphicsConnectionPackage pckg;
	pckg.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_GRAPHICSEDITOR_Package));

	Point2DD pos;
	pos.setFromJsonObject(ot::json::getObject(_document, OT_ACTION_PARAM_POSITION));

	return graphicsConnectionToConnectionRequested(pckg, pos);
}

ot::ReturnMessage ot::GraphicsHandler::handleGraphicsConnectionChanged(JsonDocument& _document) {
	GraphicsConnectionCfg cfg;
	cfg.setFromJsonObject(json::getObject(_document, OT_ACTION_PARAM_Config));

	return graphicsConnectionChanged(cfg);
}