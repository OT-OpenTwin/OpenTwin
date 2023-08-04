//! @file GraphicsRectangularItemCfg.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsRectangularItemCfg.h"
#include "OTGui/Painter2D.h"
#include "OTGui/Painter2DFactory.h"
#include "OpenTwinCore/rJSONHelper.h"

#define OT_JSON_MEMBER_CornerRadius "CornerRadius"
#define OT_JSON_MEMBER_BorderColor "BorderColor"
#define OT_JSON_MEMBER_BorderWidth "BorderWidth"
#define OT_JSON_MEMBER_BackgroundPainter "BackgroundPainter"

ot::GraphicsRectangularItemCfg::GraphicsRectangularItemCfg(ot::Painter2D* _backgroundPainter, const ot::Color& _borderColor, int _borderWidth, int _cornerRadius) 
	: m_backgroundPainter(_backgroundPainter), m_borderColor(_borderColor), m_borderWidth(_borderWidth), m_cornerRadius(_cornerRadius)
{

}

ot::GraphicsRectangularItemCfg::~GraphicsRectangularItemCfg() {
	if (m_backgroundPainter) delete m_backgroundPainter;
}

void ot::GraphicsRectangularItemCfg::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	GraphicsItemCfg::addToJsonObject(_document, _object);
	ot::rJSON::add(_document, _object, OT_SimpleFactoryJsonKey, "GraphicsRectangularItemCfg");

	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_CornerRadius, m_cornerRadius);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_BorderWidth, m_borderWidth);

	OT_rJSON_createValueObject(borderColorObj);
	m_borderColor.addToJsonObject(_document, borderColorObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_BorderColor, borderColorObj);

	if (m_backgroundPainter) {
		OT_rJSON_createValueObject(backgroundPainterObj);
		m_backgroundPainter->addToJsonObject(_document, backgroundPainterObj);
		ot::rJSON::add(_document, _object, OT_JSON_MEMBER_BackgroundPainter, backgroundPainterObj);
	}
	else {
		OT_rJSON_createValueNull(backgroundPainterObj);
		ot::rJSON::add(_document, _object, OT_JSON_MEMBER_BackgroundPainter, backgroundPainterObj);
	}
}

void ot::GraphicsRectangularItemCfg::setFromJsonObject(OT_rJSON_val& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);

	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_CornerRadius, Int);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_BorderWidth, Int);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_BorderColor, Object);
	OT_rJSON_checkMemberExists(_object, OT_JSON_MEMBER_BackgroundPainter);

	m_cornerRadius = _object[OT_JSON_MEMBER_CornerRadius].GetInt();
	m_borderWidth = _object[OT_JSON_MEMBER_BorderWidth].GetInt();

	OT_rJSON_val borderColorObj = _object[OT_JSON_MEMBER_BorderColor].GetObject();
	m_borderColor.setFromJsonObject(borderColorObj);

	if (_object[OT_JSON_MEMBER_BackgroundPainter].IsObject()) {
		OT_rJSON_val backgroundPainterObj = _object[OT_JSON_MEMBER_BackgroundPainter].GetObject();
		setBackgroundPainer(ot::Painter2DFactory::painter2DFromJson(backgroundPainterObj));
	}
	else if (_object[OT_JSON_MEMBER_BackgroundPainter].IsNull()) {
		setBackgroundPainer(nullptr);
	}
	else {
		OT_LOG_E("Background painter member is not an object (or null)");
		throw std::exception("JSON object member invalid type");
	}
}

void ot::GraphicsRectangularItemCfg::setBackgroundPainer(ot::Painter2D* _painter) {
	if (_painter == m_backgroundPainter) return;
	if (m_backgroundPainter) delete m_backgroundPainter;
	m_backgroundPainter = _painter;
}

// Register at class factory
static ot::SimpleFactoryRegistrar<ot::GraphicsRectangularItemCfg> rectItemCfg("GraphicsRectangularItemCfg");
