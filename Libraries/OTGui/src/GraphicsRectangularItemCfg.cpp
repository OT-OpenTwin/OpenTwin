//! @file GraphicsRectangularItemCfg.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsRectangularItemCfg.h"
#include "OTGui/Painter2D.h"
#include "OTGui/FillPainter2D.h"

#define OT_JSON_MEMBER_Size "Size"
#define OT_JSON_MEMBER_Border "Border"
#define OT_JSON_MEMBER_CornerRadius "CornerRadius"
#define OT_JSON_MEMBER_BackgroundPainter "BackgroundPainter"

static ot::SimpleFactoryRegistrar<ot::GraphicsRectangularItemCfg> rectItemCfg(OT_SimpleFactoryJsonKeyValue_GraphicsRectangularItemCfg);

ot::GraphicsRectangularItemCfg::GraphicsRectangularItemCfg(ot::Painter2D* _backgroundPainter, int _cornerRadius)
	: m_backgroundPainter(_backgroundPainter), m_cornerRadius(_cornerRadius)
{
	if (m_backgroundPainter == nullptr) {
		// If no background painter was provided, we set a transparent background as default
		m_backgroundPainter = new ot::FillPainter2D(ot::Color(0, 0, 0, 0));
	}
}

ot::GraphicsRectangularItemCfg::~GraphicsRectangularItemCfg() {
	if (m_backgroundPainter) delete m_backgroundPainter;
}

void ot::GraphicsRectangularItemCfg::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	OTAssertNullptr(m_backgroundPainter);

	GraphicsItemCfg::addToJsonObject(_document, _object);

	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_CornerRadius, m_cornerRadius);

	OT_rJSON_createValueObject(sizeObj);
	m_size.addToJsonObject(_document, sizeObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Size, sizeObj);

	OT_rJSON_createValueObject(backgroundPainterObj);
	m_backgroundPainter->addToJsonObject(_document, backgroundPainterObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_BackgroundPainter, backgroundPainterObj);

	OT_rJSON_createValueObject(borderObj);
	m_border.addToJsonObject(_document, borderObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Border, borderObj);
}

void ot::GraphicsRectangularItemCfg::setFromJsonObject(OT_rJSON_val& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);

	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_CornerRadius, Int);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Border, Object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_BackgroundPainter, Object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Size, Object);

	m_cornerRadius = _object[OT_JSON_MEMBER_CornerRadius].GetInt();

	OT_rJSON_val backgroundPainterObj = _object[OT_JSON_MEMBER_BackgroundPainter].GetObject();
	ot::Painter2D* p = ot::SimpleFactory::instance().createType<ot::Painter2D>(backgroundPainterObj);
	p->setFromJsonObject(backgroundPainterObj);
	this->setBackgroundPainer(p);

	OT_rJSON_val borderObj = _object[OT_JSON_MEMBER_Border].GetObject();
	m_border.setFromJsonObject(borderObj);

	OT_rJSON_val sizeObj = _object[OT_JSON_MEMBER_Size].GetObject();
	m_size.setFromJsonObject(sizeObj);
}

void ot::GraphicsRectangularItemCfg::setBackgroundPainer(ot::Painter2D* _painter) {
	if (_painter == m_backgroundPainter) return;
	if (m_backgroundPainter) delete m_backgroundPainter;
	m_backgroundPainter = _painter;
}
