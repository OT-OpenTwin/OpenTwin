// @otlicense
// File: DialogHandler.cpp
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
#include "OTGuiAPI/DialogHandler.h"
#include "OTCommunication/ActionTypes.h"

ot::JsonDocument ot::DialogHandler::createDialogRequest(const PropertyDialogCfg& _cfg, const std::string& _callbackAction)
{
	return createDialogRequest(&_cfg, OT_ACTION_CMD_UI_PropertyDialog, _callbackAction, std::nullopt);
}

ot::JsonDocument ot::DialogHandler::createDialogRequest(const DialogCfg* _cfg, const std::string& _requestAction, const std::string& _callbackAction, const std::optional<std::string>& _additionalInfo)
{
	JsonDocument doc;

	doc.AddMember(OT_ACTION_MEMBER, JsonString(_requestAction, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Config, JsonObject(_cfg, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_CallbackAction, JsonString(_callbackAction, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SENDER_URL, JsonString(ThisService::instance().getServiceURL(), doc.GetAllocator()), doc.GetAllocator());
	if (_additionalInfo.has_value()) {
		doc.AddMember(OT_ACTION_PARAM_Info, JsonString(_additionalInfo.value(), doc.GetAllocator()), doc.GetAllocator());
	}

	return doc;
}
