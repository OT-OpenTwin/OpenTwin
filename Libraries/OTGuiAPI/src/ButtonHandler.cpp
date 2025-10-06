//! @file ButtonHandler.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin System header
#include "OTSystem/OTAssert.h"

// OpenTwin Core header
#include "OTCore/LogDispatcher.h"

// OpenTwin GuiAPI header
#include "OTGuiAPI/ButtonHandler.h"

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

std::string ot::ButtonHandler::handleButtonClicked(JsonDocument& _document) {
	std::string action = ot::json::getString(_document, OT_ACTION_PARAM_MODEL_ActionName);
	
	auto it = m_callbacks.find(action);
	if (it != m_callbacks.end()) {
		it->second();
	}
	else {
		this->buttonClicked(action);
	}

	return std::string();
}