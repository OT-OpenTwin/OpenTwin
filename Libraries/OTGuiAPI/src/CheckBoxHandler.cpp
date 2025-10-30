// @otlicense
// File: CheckBoxHandler.cpp
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
#include "OTGuiAPI/CheckBoxHandler.h"

ot::CheckBoxHandler::CheckBoxHandler() {
	if (!m_actionHandler.connectAction(OT_ACTION_CMD_LineEditChanged, this, &CheckBoxHandler::handleToolBarCheckBoxChanged, ot::SECURE_MESSAGE_TYPES, ActionDispatcher::ExpectMultiple)) {
		OT_LOG_EA("Failed to register check box changed handler");
	}
}

void ot::CheckBoxHandler::connectToolBarCheckBox(const std::string& _checkBoxKey, void(*_callback)(bool)) {
	OTAssertNullptr(_callback);
	OTAssert(m_callbacks.find(_checkBoxKey) == m_callbacks.end(), "A callback is already registered for the given tool bar check box key");
	m_callbacks.insert_or_assign(_checkBoxKey, _callback);
}

void ot::CheckBoxHandler::connectToolBarCheckBox(const std::string& _checkBoxKey, const std::function<void(bool)>& _callback) {
	OTAssert(m_callbacks.find(_checkBoxKey) == m_callbacks.end(), "A callback is already registered for the given tool bar check box key");
	m_callbacks.insert_or_assign(_checkBoxKey, _callback);
}

bool ot::CheckBoxHandler::disconnectToolBarCheckBox(const std::string& _checkBoxKey) {
	return m_callbacks.erase(_checkBoxKey) > 0;
}

void ot::CheckBoxHandler::toolBarCheckBoxChanged(const std::string& _checkBoxKey, bool _isChecked) {
	
}

void ot::CheckBoxHandler::handleToolBarCheckBoxChanged(JsonDocument& _document) {
	std::string action = ot::json::getString(_document, OT_ACTION_PARAM_NAME);
	bool isChecked = false;

	auto it = m_callbacks.find(action);
	if (it != m_callbacks.end()) {
		it->second(isChecked);
	}
	else {
		this->toolBarCheckBoxChanged(action, isChecked);
	}
}