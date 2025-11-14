// @otlicense
// File: TextEditorActionHandler.cpp
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
#include "OTGuiAPI/TextEditorActionHandler.h"

ot::TextEditorActionHandler::TextEditorActionHandler(ActionDispatcherBase* _dispatcher) 
	: m_actionHandler(_dispatcher)
{
	m_actionHandler.connectAction(OT_ACTION_CMD_UI_TEXTEDITOR_SaveRequest, this, &TextEditorActionHandler::handleTextEditorSaveRequested);
}

ot::JsonDocument ot::TextEditorActionHandler::createTextEditorSaveRequestDocument(const std::string& _entityName, const std::string& _content, size_t _nextChunkStartIndex) {
	ot::JsonDocument doc;

	doc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_UI_TEXTEDITOR_SaveRequest, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_TEXTEDITOR_Name, JsonString(_entityName.c_str(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_TEXTEDITOR_Text, JsonString(_content.c_str(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Index, _nextChunkStartIndex, doc.GetAllocator());

	return doc;
}

ot::ReturnMessage ot::TextEditorActionHandler::handleTextEditorSaveRequested(JsonDocument& _doc) {
	const std::string entityName = json::getString(_doc, OT_ACTION_PARAM_TEXTEDITOR_Name);
	const std::string textContent = json::getString(_doc, OT_ACTION_PARAM_TEXTEDITOR_Text);
	size_t nextChunkStartIndex = json::getUInt64(_doc, OT_ACTION_PARAM_Index, 0);
	
	return this->textEditorSaveRequested(entityName, textContent, nextChunkStartIndex);
}
