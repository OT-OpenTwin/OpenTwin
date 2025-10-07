//! @file ButtonHandler.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTSystem/OTAssert.h"
#include "OTCore/LogDispatcher.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionDispatcher.h"
#include "OTCommunication/ActionHandleConnector.h"
#include "OTGuiAPI/ButtonHandler.h"

ot::ButtonHandler::ButtonHandler() {
	m_actionHandleConnector = ActionDispatcher::instance().connect(OT_ACTION_CMD_MODEL_ExecuteAction, ot::SECURE_MESSAGE_TYPES, this, &ButtonHandler::handleButtonClicked, ActionDispatcher::ExpectMultiple);
}

void ot::ButtonHandler::connectButton(void(*_callback)(), const std::string& _buttonKey) {
    OTAssertNullptr(_callback);
	m_callbacks.insert_or_assign(_buttonKey, _callback);
}

void ot::ButtonHandler::connectButton(const std::function<void()>& _callback, const std::string& _buttonKey) {
	m_callbacks.insert_or_assign(_buttonKey, _callback);
}

bool ot::ButtonHandler::disconnectButton(const std::string& _buttonKey) {
    return m_callbacks.erase(_buttonKey) > 0;
}

void ot::ButtonHandler::buttonClicked(const std::string& _buttonKey) {
	OT_LOG_W("Unhandled button click event { \"ButtonKey\": \"" + _buttonKey + "\" }");
}

void ot::ButtonHandler::handleButtonClicked(JsonDocument& _document) {
	std::string action = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ActionName);
	
	auto it = m_callbacks.find(action);
	if (it != m_callbacks.end()) {
		it->second();
	}
	else {
		this->buttonClicked(action);
	}
}