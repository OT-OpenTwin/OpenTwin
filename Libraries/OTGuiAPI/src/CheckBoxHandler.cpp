// @otlicense

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