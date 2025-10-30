// @otlicense
// File: ButtonHandler.cpp
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
#include "OTCore/LogDispatcher.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionDispatcher.h"
#include "OTCommunication/ActionHandleConnector.h"
#include "OTGuiAPI/ButtonHandler.h"

ot::ButtonHandler::ButtonHandler(ActionDispatcherBase* _dispatcher)
	: m_actionHandler(_dispatcher)
{
	if (!m_actionHandler.connectAction(OT_ACTION_CMD_ButtonPressed, this, &ButtonHandler::handleToolBarButtonClicked, ot::SECURE_MESSAGE_TYPES, ActionDispatcher::ExpectMultiple)) {
		OT_LOG_EA("Failed to register button click handler");
	}
}

void ot::ButtonHandler::connectToolBarButton(const std::string& _buttonKey, void(*_callback)()) {
    OTAssertNullptr(_callback);
	OTAssert(m_callbacks.find(_buttonKey) == m_callbacks.end(), "A callback is already registered for the given tool bar button key");
	m_callbacks.insert_or_assign(_buttonKey, _callback);
}

void ot::ButtonHandler::connectToolBarButton(const std::string& _buttonKey, const std::function<void()>& _callback) {
	OTAssert(m_callbacks.find(_buttonKey) == m_callbacks.end(), "A callback is already registered for the given tool bar button key");
	m_callbacks.insert_or_assign(_buttonKey, _callback);
}

bool ot::ButtonHandler::disconnectToolBarButton(const std::string& _buttonKey) {
    return m_callbacks.erase(_buttonKey) > 0;
}

void ot::ButtonHandler::toolBarButtonClicked(const std::string& _buttonKey) {
	
}

void ot::ButtonHandler::handleToolBarButtonClicked(JsonDocument& _document) {
	std::string action = ot::json::getString(_document, OT_ACTION_PARAM_NAME);
	
	auto it = m_callbacks.find(action);
	if (it != m_callbacks.end()) {
		it->second();
	}
	else {
		this->toolBarButtonClicked(action);
	}
}