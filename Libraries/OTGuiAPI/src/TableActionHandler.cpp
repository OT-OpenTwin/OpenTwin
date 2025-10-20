//! @file TableActionHandler.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGuiAPI/TableActionHandler.h"

ot::TableActionHandler::TableActionHandler(ActionDispatcherBase* _dispatcher) {
	m_actionHandler.connectAction(OT_ACTION_CMD_UI_TABLE_SaveRequest, this, &TableActionHandler::handleTableSaveRequested);
}

void ot::TableActionHandler::handleTableSaveRequested(JsonDocument& _doc) {
	ot::TableCfg config;
	config.setFromJsonObject(json::getObject(_doc, OT_ACTION_PARAM_Config));

	this->tableSaveRequested(config);
}
