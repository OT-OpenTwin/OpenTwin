//! @file TextEditorActionHandler.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGuiAPI/TextEditorActionHandler.h"

ot::TextEditorActionHandler::TextEditorActionHandler(ActionDispatcherBase* _dispatcher) {
	m_actionHandler.connectAction(OT_ACTION_CMD_UI_TEXTEDITOR_SaveRequest, this, &TextEditorActionHandler::handleTextEditorSaveRequested);
}

void ot::TextEditorActionHandler::handleTextEditorSaveRequested(JsonDocument& _doc) {
	const std::string entityName = _doc[OT_ACTION_PARAM_TEXTEDITOR_Name].GetString();
	const std::string textContent = _doc[OT_ACTION_PARAM_TEXTEDITOR_Text].GetString();
	
	this->textEditorSaveRequested(entityName, textContent);
}
