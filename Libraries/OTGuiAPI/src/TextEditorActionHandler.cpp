// @otlicense

//! @file TextEditorActionHandler.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGuiAPI/TextEditorActionHandler.h"

ot::TextEditorActionHandler::TextEditorActionHandler(ActionDispatcherBase* _dispatcher) 
	: m_actionHandler(_dispatcher)
{
	m_actionHandler.connectAction(OT_ACTION_CMD_UI_TEXTEDITOR_SaveRequest, this, &TextEditorActionHandler::handleTextEditorSaveRequested);
}

ot::JsonDocument ot::TextEditorActionHandler::createTextEditorSaveRequestDocument(const std::string& _entityName, const std::string& _content) {
	ot::JsonDocument doc;

	doc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_TEXTEDITOR_SaveRequest, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_TEXTEDITOR_Name, JsonString(_entityName.c_str(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_TEXTEDITOR_Text, JsonString(_content.c_str(), doc.GetAllocator()), doc.GetAllocator());

	return doc;
}

ot::ReturnMessage ot::TextEditorActionHandler::handleTextEditorSaveRequested(JsonDocument& _doc) {
	const std::string entityName = json::getString(_doc, OT_ACTION_PARAM_TEXTEDITOR_Name);
	const std::string textContent = json::getString(_doc, OT_ACTION_PARAM_TEXTEDITOR_Text);
	
	return this->textEditorSaveRequested(entityName, textContent);
}
