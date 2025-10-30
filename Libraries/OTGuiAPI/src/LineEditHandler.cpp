// @otlicense
// File: LineEditHandler.cpp
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
#include "OTGuiAPI/LineEditHandler.h"

ot::LineEditHandler::LineEditHandler() {
	if (!m_actionHandler.connectAction(OT_ACTION_CMD_LineEditChanged, this, &LineEditHandler::handleToolBarLineEditChanged, ot::SECURE_MESSAGE_TYPES, ActionDispatcher::ExpectMultiple)) {
		OT_LOG_EA("Failed to register line edit changed handler");
	}
}

void ot::LineEditHandler::connectToolBarLineEdit(const std::string& _lineEditKey, void(*_callback)(const std::string&)) {
	OTAssertNullptr(_callback);
	OTAssert(m_callbacks.find(_lineEditKey) == m_callbacks.end(), "A callback is already registered for the given tool bar line edit key");
	m_callbacks.insert_or_assign(_lineEditKey, _callback);
}

void ot::LineEditHandler::connectToolBarLineEdit(const std::string& _lineEditKey, const std::function<void(const std::string&)>& _callback) {
	OTAssert(m_callbacks.find(_lineEditKey) == m_callbacks.end(), "A callback is already registered for the given tool bar line edit key");
	m_callbacks.insert_or_assign(_lineEditKey, _callback);
}

bool ot::LineEditHandler::disconnectToolBarLineEdit(const std::string& _lineEditKey) {
	return m_callbacks.erase(_lineEditKey) > 0;
}

void ot::LineEditHandler::toolBarLineEditTextChanged(const std::string& _lineEditKey, const std::string& _newText) {
	
}

void ot::LineEditHandler::handleToolBarLineEditChanged(JsonDocument& _document) {
	std::string action = ot::json::getString(_document, OT_ACTION_PARAM_NAME);
	const std::string newText = ot::json::getString(_document, OT_ACTION_PARAM_UI_CONTROL_ObjectText);

	auto it = m_callbacks.find(action);
	if (it != m_callbacks.end()) {
		it->second(newText);
	}
	else {
		this->toolBarLineEditTextChanged(action, newText);
	}
}