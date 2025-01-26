//! @file TextEditorCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/RuntimeTests.h"
#include "OTGui/TextEditorCfg.h"

#if OT_TESTING_GLOBAL_AllTestsEnabled==true
#define OT_TESTING_LOCAL_TEXTEDITORCFG_PERFORMANCETEST_ENABLED OT_TESTING_GLOBAL_AllTestsEnabled
#elif OT_TESTING_GLOBAL_RuntimeTestingEnabled==true
#define OT_TESTING_LOCAL_TEXTEDITORCFG_PERFORMANCETEST_ENABLED OT_TESTING_GLOBAL_RuntimeTestingEnabled
#else
#define OT_TESTING_LOCAL_TEXTEDITORCFG_PERFORMANCETEST_ENABLED false
#endif 

#if OT_TESTING_LOCAL_TEXTEDITORCFG_PERFORMANCETEST_ENABLED==true
#define OT_TEST_TEXTEDITORCFG_Interval(___testText) OT_TEST_Interval(ot_intern_texteditorcfg_lcl_performancetest, "TextEditorCfg", ___testText)
#else
#define OT_TEST_TEXTEDITORCFG_Interval(___testText)
#endif

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
	OT_TEST_TEXTEDITORCFG_Interval("Export");

	WidgetViewBase::addToJsonObject(_object, _allocator);

	_object.AddMember("Text", JsonString(m_text, _allocator), _allocator);
	_object.AddMember("Syntax", JsonString(ot::toString(m_syntax), _allocator), _allocator);
}

void ot::TextEditorCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	OT_TEST_TEXTEDITORCFG_Interval("Import");

	WidgetViewBase::setFromJsonObject(_object);
	
	m_text = json::getString(_object, "Text");
	m_syntax = stringToDocumentSyntax(json::getString(_object, "Syntax"));
}
