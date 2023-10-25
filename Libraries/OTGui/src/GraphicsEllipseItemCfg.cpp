//! @file GraphicsEllipseItemCfg.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsEllipseItemCfg.h"
#include "OTGui/Painter2D.h"

#define OT_JSON_MEMBER_Border "Border"
#define OT_JSON_MEMBER_RadiusX "RadiusX"
#define OT_JSON_MEMBER_RadiusY "RadiusY"
#define OT_JSON_MEMBER_BackgroundPainter "BackgroundPainter"

static ot::SimpleFactoryRegistrar<ot::GraphicsEllipseItemCfg> ellipseItemCfg(OT_SimpleFactoryJsonKeyValue_GraphicsEllipseItemCfg);

ot::GraphicsEllipseItemCfg::GraphicsEllipseItemCfg(int _radiusX, int _radiusY, ot::Painter2D* _backgroundPainter)
	: m_backgroundPainter(_backgroundPainter), m_radiusX(_radiusX), m_radiusY(_radiusY)
{
	if (m_backgroundPainter == nullptr) {
		// If no background painter was provided, we set a transparent background as default
		m_backgroundPainter = new ot::FillPainter2D(ot::Color(0, 0, 0, 0));
	}
}

ot::GraphicsEllipseItemCfg::~GraphicsEllipseItemCfg() {
	if (m_backgroundPainter) delete m_backgroundPainter;
}

void ot::GraphicsEllipseItemCfg::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	OTAssertNullptr(m_backgroundPainter);

	GraphicsItemCfg::addToJsonObject(_document, _object);

	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_RadiusX, m_radiusX);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_RadiusY, m_radiusY);

	OT_rJSON_createValueObject(backgroundPainterObj);
	m_backgroundPainter->addToJsonObject(_document, backgroundPainterObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_BackgroundPainter, backgroundPainterObj);

	OT_rJSON_createValueObject(borderObj);
	m_border.addToJsonObject(_document, borderObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Border, borderObj);
}

void ot::GraphicsEllipseItemCfg::setFromJsonObject(OT_rJSON_val& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);

	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_RadiusX, Int);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_RadiusY, Int);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Border, Object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_BackgroundPainter, Object);

	m_radiusX = _object[OT_JSON_MEMBER_RadiusX].GetInt();
	m_radiusY = _object[OT_JSON_MEMBER_RadiusY].GetInt();

	OT_rJSON_val backgroundPainterObj = _object[OT_JSON_MEMBER_BackgroundPainter].GetObject();
	ot::Painter2D* p = ot::SimpleFactory::instance().createType<ot::Painter2D>(backgroundPainterObj);
	p->setFromJsonObject(backgroundPainterObj);
	this->setBackgroundPainer(p);

	OT_rJSON_val borderObj = _object[OT_JSON_MEMBER_Border].GetObject();
	m_border.setFromJsonObject(borderObj);
}

void ot::GraphicsEllipseItemCfg::setBackgroundPainer(ot::Painter2D* _painter) {
	if (_painter == m_backgroundPainter) return;
	if (m_backgroundPainter) delete m_backgroundPainter;
	m_backgroundPainter = _painter;
}
