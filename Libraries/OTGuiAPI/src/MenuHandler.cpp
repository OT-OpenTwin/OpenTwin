// @otlicense
// File: ContextMenuHandler.cpp
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
#include "OTSystem/OTAssert.h"
#include "OTCore/Logging/LogDispatcher.h"
#include "OTGui/Menu/MenuCfg.h"
#include "OTGui/Event/MenuRequestData.h"
#include "OTGui/Event/MenuRequestEvent.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Dispatch/ActionDispatcher.h"
#include "OTCommunication/Handler/ActionHandleConnector.h"
#include "OTGuiAPI/MenuHandler.h"

ot::MenuHandler::MenuHandler(ActionDispatcherBase* _dispatcher)
	: m_actionHandler(_dispatcher)
{
	if (!m_actionHandler.connectAction(OT_ACTION_CMD_UI_CreateContextMenu, this, &MenuHandler::handleMenuRequested, ot::SECURE_MESSAGE_TYPES))
	{
		OT_LOG_EA("Failed to register context menu request handler");
	}
}

ot::JsonDocument ot::MenuHandler::createMenuRequestedDocument(const MenuRequestEvent& _eventData)
{
	JsonDocument result;
	result.AddMember(OT_ACTION_MEMBER, JsonStringRef(OT_ACTION_CMD_UI_CreateContextMenu), result.GetAllocator());
	result.AddMember(OT_ACTION_PARAM_Event, JsonObject(_eventData, result.GetAllocator()), result.GetAllocator());

	return result;
}

ot::MenuCfg ot::MenuHandler::menuRequested(const MenuRequestEvent& _event)
{
	OT_LOG_W("Menu requested, handler in use, request not implemented, returning empty menu");
	return MenuCfg();
}

ot::ReturnMessage ot::MenuHandler::handleMenuRequested(JsonDocument& _document)
{
	MenuRequestEvent event = json::getObject(_document, OT_ACTION_PARAM_Event);
	MenuCfg menu = menuRequested(event);
	return ReturnMessage::ok(menu.toJson());
}