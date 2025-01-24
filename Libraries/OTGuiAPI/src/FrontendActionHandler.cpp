//! @file FrontendActionHandler.cpp
//!
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGuiAPI/FrontendActionHandler.h"

std::string ot::FrontendActionHandler::handleKeySequenceActivated(JsonDocument& _document) {
	std::string keySequence = json::getString(_document, OT_ACTION_PARAM_UI_KeySequence);
	this->keySequenceActivated(keySequence);
	return OT_ACTION_RETURN_VALUE_OK;
}

std::string ot::FrontendActionHandler::handleSettingsItemChanged(JsonDocument& _document) {
	
	return "";
}
