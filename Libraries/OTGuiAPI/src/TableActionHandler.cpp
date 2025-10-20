//! @file TableActionHandler.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGuiAPI/TableActionHandler.h"

ot::TableActionHandler::TableActionHandler(ActionDispatcherBase* _dispatcher)
	: m_actionHandler(_dispatcher)
{
	m_actionHandler.connectAction(OT_ACTION_CMD_UI_TABLE_SaveRequest, this, &TableActionHandler::handleTableSaveRequested);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Public: Action document helper

ot::JsonDocument ot::TableActionHandler::createTableSaveRequestDocument(const ot::TableCfg& _config) {
	JsonDocument doc;
	
	doc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_TABLE_SaveRequest, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Config, JsonObject(_config, doc.GetAllocator()), doc.GetAllocator());

	return doc;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Action handling

ot::ReturnMessage ot::TableActionHandler::handleTableSaveRequested(JsonDocument& _doc) {
	ot::TableCfg config;
	config.setFromJsonObject(json::getObject(_doc, OT_ACTION_PARAM_Config));

	return this->tableSaveRequested(config);
}
