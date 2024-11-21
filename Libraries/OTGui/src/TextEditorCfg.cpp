//! @file TextEditorCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/TextEditorCfg.h"

ot::TextEditorCfg::TextEditorCfg() 
	: WidgetViewBase(WidgetViewBase::ViewText, WidgetViewBase::ViewIsCentral | WidgetViewBase::ViewIsCloseable), m_syntax(DocumentSyntax::PlainText)
{}

ot::TextEditorCfg::TextEditorCfg(const TextEditorCfg& _other)
	: WidgetViewBase(_other), m_text(_other.m_text), m_syntax(_other.m_syntax)
{}

ot::TextEditorCfg::~TextEditorCfg() {}

ot::TextEditorCfg& ot::TextEditorCfg::operator = (const TextEditorCfg& _other) {
	WidgetViewBase::operator=(_other);

	if (this != &_other) {
		m_text = _other.m_text;
		m_syntax = _other.m_syntax;

	}
	
	return *this;
}

void ot::TextEditorCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	WidgetViewBase::addToJsonObject(_object, _allocator);

	_object.AddMember("Text", JsonString(m_text, _allocator), _allocator);
	_object.AddMember("Syntax", JsonString(ot::toString(m_syntax), _allocator), _allocator);
}

void ot::TextEditorCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	WidgetViewBase::setFromJsonObject(_object);
	
	m_text = json::getString(_object, "Text");
	m_syntax = toDocumentSyntax(json::getString(_object, "Syntax"));
}
