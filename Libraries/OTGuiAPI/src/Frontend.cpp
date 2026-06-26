// @otlicense
// File: Frontend.cpp
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
#include "OTCore/ThisService.h"
#include "OTCore/Logging/Logger.h"
#include "OTCommunication/ActionTypes.h"
#include "OTGuiAPI/Frontend.h"
#include "OTGuiAPI/GuiAPIManager.h"

// ###########################################################################################################################################################################################################################################################################################################################

// File operations

bool ot::Frontend::requestFileForReading(const std::string& _callbackAction, const std::string& _dialogTitle, const std::string& _fileMask, bool _loadContent, bool _loadMultiple, const std::string& _additionalInfo) {
	JsonDocument request;
	request.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_RequestFileForReading, request.GetAllocator()), request.GetAllocator());
	request.AddMember(OT_ACTION_PARAM_SENDER_URL, JsonString(ThisService::instance().getServiceURL(), request.GetAllocator()), request.GetAllocator());
	request.AddMember(OT_ACTION_PARAM_CallbackAction, JsonString(_callbackAction, request.GetAllocator()), request.GetAllocator());
	request.AddMember(OT_ACTION_PARAM_UI_DIALOG_TITLE, JsonString(_dialogTitle, request.GetAllocator()), request.GetAllocator());
	request.AddMember(OT_ACTION_PARAM_FILE_Mask, JsonString(_fileMask, request.GetAllocator()), request.GetAllocator());
	request.AddMember(OT_ACTION_PARAM_FILE_LoadMultiple, _loadMultiple, request.GetAllocator());
	request.AddMember(OT_ACTION_PARAM_FILE_LoadContent, _loadContent, request.GetAllocator());
	request.AddMember(OT_ACTION_PARAM_Info, JsonString(_additionalInfo, request.GetAllocator()), request.GetAllocator());

	return GuiAPIManager::instance().sendQueuedRequestToFrontend(request.toJson());
}

bool ot::Frontend::requestFileForWriting(const std::string& _callbackAction, const std::string& _dialogTitle, const std::string& _fileMask, const std::string& _additionalInfo) {
	JsonDocument request;
	request.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_SelectFileForStoring, request.GetAllocator()), request.GetAllocator());
	request.AddMember(OT_ACTION_PARAM_SENDER_URL, JsonString(ThisService::instance().getServiceURL(), request.GetAllocator()), request.GetAllocator());
	request.AddMember(OT_ACTION_PARAM_CallbackAction, JsonString(_callbackAction, request.GetAllocator()), request.GetAllocator());
	request.AddMember(OT_ACTION_PARAM_UI_DIALOG_TITLE, JsonString(_dialogTitle, request.GetAllocator()), request.GetAllocator());
	request.AddMember(OT_ACTION_PARAM_FILE_Mask, JsonString(_fileMask, request.GetAllocator()), request.GetAllocator());
	request.AddMember(OT_ACTION_PARAM_Info, JsonString(_additionalInfo, request.GetAllocator()), request.GetAllocator());

	return GuiAPIManager::instance().sendQueuedRequestToFrontend(request.toJson());
}

bool ot::Frontend::writeDataToFile(const std::string& _dialogTitle, const std::string& _filePath, const std::string& _content, unsigned long long _uncompressedContentLength) {
	JsonDocument request;
	request.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_SaveFileContent, request.GetAllocator()), request.GetAllocator());
	request.AddMember(OT_ACTION_PARAM_SENDER_URL, JsonString(ThisService::instance().getServiceURL(), request.GetAllocator()), request.GetAllocator());
	request.AddMember(OT_ACTION_PARAM_UI_DIALOG_TITLE, JsonString(_dialogTitle, request.GetAllocator()), request.GetAllocator());
	request.AddMember(OT_ACTION_PARAM_FILE_OriginalName, JsonString(_filePath, request.GetAllocator()), request.GetAllocator());
	request.AddMember(OT_ACTION_PARAM_FILE_Content, JsonString(_content, request.GetAllocator()), request.GetAllocator());
	request.AddMember(OT_ACTION_PARAM_FILE_Content_UncompressedDataLength, _uncompressedContentLength, request.GetAllocator());

	return GuiAPIManager::instance().sendQueuedRequestToFrontend(request.toJson());
}

bool ot::Frontend::setEntitySelected(const std::string& _entityName, bool _selected, bool _clearSelection, bool _expandAllParents)
{
	return setEntitiesSelected(std::list<std::string>{ _entityName }, _selected, _clearSelection, _expandAllParents);
}

bool ot::Frontend::setEntitiesSelected(const std::list<std::string>& _entityNames, bool _selected, bool _clearSelection, bool _expandAllParents)
{
	ot::JsonDocument uiDoc;
	uiDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_VIEW_SetEntitySelected, uiDoc.GetAllocator()), uiDoc.GetAllocator());
	uiDoc.AddMember(OT_ACTION_PARAM_IsSelected, _selected, uiDoc.GetAllocator());
	uiDoc.AddMember(OT_ACTION_PARAM_ExpandParents, _expandAllParents, uiDoc.GetAllocator());
	uiDoc.AddMember(OT_ACTION_PARAM_ClearSelection, _clearSelection, uiDoc.GetAllocator());
	uiDoc.AddMember(OT_ACTION_PARAM_NAME, JsonArray(_entityNames, uiDoc.GetAllocator()), uiDoc.GetAllocator());

	return GuiAPIManager::instance().sendQueuedRequestToFrontend(uiDoc.toJson());
}

bool ot::Frontend::setEntitySelected(UID _entityID, bool _selected, bool _clearSelection, bool _expandAllParents)
{
	return setEntitiesSelected(UIDList{ _entityID }, _selected, _clearSelection, _expandAllParents);
}

bool ot::Frontend::setEntitiesSelected(const UIDList& _entityIDs, bool _selected, bool _clearSelection, bool _expandAllParents)
{
	ot::JsonDocument uiDoc;
	uiDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_VIEW_SetEntitySelected, uiDoc.GetAllocator()), uiDoc.GetAllocator());
	uiDoc.AddMember(OT_ACTION_PARAM_IsSelected, _selected, uiDoc.GetAllocator());
	uiDoc.AddMember(OT_ACTION_PARAM_ExpandParents, _expandAllParents, uiDoc.GetAllocator());
	uiDoc.AddMember(OT_ACTION_PARAM_ClearSelection, _clearSelection, uiDoc.GetAllocator());
	uiDoc.AddMember(OT_ACTION_PARAM_MODEL_ITM_ID, JsonArray(_entityIDs, uiDoc.GetAllocator()), uiDoc.GetAllocator());

	return GuiAPIManager::instance().sendQueuedRequestToFrontend(uiDoc.toJson());
}

bool ot::Frontend::promptChoice(const std::string& _callbackAction, const std::string& _title, const std::string& _message, ot::MessageDialogCfg::BasicIcon _icon, ot::MessageDialogCfg::BasicButtons _buttons, const std::string& _additionalInfo)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_PromptInformation, doc.GetAllocator()), doc.GetAllocator());

	ot::MessageDialogCfg config;
	config.setText(_message);
	config.setIcon(_icon);
	config.setButtons(_buttons);
	config.setTitle(_title);
	ot::JsonObject configObj;
	config.addToJsonObject(configObj, doc.GetAllocator());

	doc.AddMember(OT_ACTION_PARAM_SENDER_URL, JsonString(ThisService::instance().getServiceURL(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_CallbackAction, ot::JsonString(_callbackAction, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Config, ot::JsonObject(config, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Info, ot::JsonString(_additionalInfo, doc.GetAllocator()), doc.GetAllocator());

	return GuiAPIManager::instance().sendQueuedRequestToFrontend(doc.toJson());
}

bool ot::Frontend::displayTemporaryStateMessage(const std::string& _message, int _displayDurationMs)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_DisplayStateMessage, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Text, ot::JsonString(_message, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Timeout, _displayDurationMs, doc.GetAllocator());

	return GuiAPIManager::instance().sendQueuedRequestToFrontend(doc.toJson());
}
