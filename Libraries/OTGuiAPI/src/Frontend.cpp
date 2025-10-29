// @otlicense

//! @file Frontend.cpp
//!
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/ThisService.h"
#include "OTCore/LogDispatcher.h"
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
