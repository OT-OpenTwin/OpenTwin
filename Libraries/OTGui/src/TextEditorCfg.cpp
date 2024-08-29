//! @file TextEditorCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/TextEditorCfg.h"

ot::TextEditorCfg::TextEditorCfg() 
	: m_syntax(DocumentSyntax::PlainText)
{}

ot::TextEditorCfg::TextEditorCfg(const TextEditorCfg& _other)
	: m_name(_other.m_name), m_title(_other.m_title), m_text(_other.m_text), m_syntax(_other.m_syntax)
{}

ot::TextEditorCfg::~TextEditorCfg() {}

ot::TextEditorCfg& ot::TextEditorCfg::operator = (const TextEditorCfg& _other) {
	if (this == &_other) return *this;
	
	m_name = _other.m_name;
	m_title = _other.m_title;
	m_text = _other.m_text;
	m_syntax = _other.m_syntax;

	return *this;
}

void ot::TextEditorCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("Title", JsonString(m_title, _allocator), _allocator);
	_object.AddMember("Text", JsonString(m_text, _allocator), _allocator);
	_object.AddMember("Syntax", JsonString(toString(m_syntax), _allocator), _allocator);
}

void ot::TextEditorCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_name = json::getString(_object, "Name");
	m_title = json::getString(_object, "Title");
	m_text = json::getString(_object, "Text");
	m_syntax = toDocumentSyntax(json::getString(_object, "Syntax"));
}
