//! @file GraphicsTextItemCfg.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsTextItemCfg.h"
#include "OTGui/FillPainter2D.h"

#define OT_JSON_MEMBER_Text "Text"
#define OT_JSON_MEMBER_TextFont "TextFont"
#define OT_JSON_MEMBER_TextPainter "TextPainter"

static ot::SimpleFactoryRegistrar<ot::GraphicsTextItemCfg> textItemCfg(OT_SimpleFactoryJsonKeyValue_GraphicsTextItemCfg);

ot::GraphicsTextItemCfg::GraphicsTextItemCfg(const std::string& _text, const ot::Color& _textColor)
	: m_text(_text), m_textPainter(nullptr) 
{
	this->setTextColor(_textColor);
}

ot::GraphicsTextItemCfg::~GraphicsTextItemCfg() {}

void ot::GraphicsTextItemCfg::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	GraphicsItemCfg::addToJsonObject(_document, _object);

	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Text, m_text);

	OT_rJSON_createValueObject(fontObj);
	OT_rJSON_createValueObject(colorObj);

	m_textFont.addToJsonObject(_document, fontObj);
	m_textPainter->addToJsonObject(_document, colorObj);

	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_TextFont, fontObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_TextPainter, colorObj);
}

void ot::GraphicsTextItemCfg::setFromJsonObject(OT_rJSON_val& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);

	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Text, String);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_TextFont, Object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_TextPainter, Object);

	m_text = _object[OT_JSON_MEMBER_Text].GetString();

	OT_rJSON_val fontObj = _object[OT_JSON_MEMBER_TextFont].GetObject();
	OT_rJSON_val colorObj = _object[OT_JSON_MEMBER_TextPainter].GetObject();

	m_textFont.setFromJsonObject(fontObj);
	
	ot::Painter2D* p = ot::SimpleFactory::instance().createType<ot::Painter2D>(colorObj);
	p->setFromJsonObject(colorObj);
	this->setTextPainter(p);
}

void ot::GraphicsTextItemCfg::setTextColor(const ot::Color& _color) {
	this->setTextPainter(new ot::FillPainter2D(_color));
}

void ot::GraphicsTextItemCfg::setTextPainter(ot::Painter2D* _painter) {
	OTAssertNullptr(_painter);
	if (m_textPainter) {
		delete m_textPainter;
	}
	m_textPainter = _painter;
}