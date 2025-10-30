// @otlicense
// File: TableActionHandler.cpp
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
