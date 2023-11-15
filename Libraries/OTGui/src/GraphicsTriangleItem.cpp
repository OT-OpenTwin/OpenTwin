//! @file GraphicsTriangleItemCfg.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsTriangleItemCfg.h"
#include "OTGui/Painter2D.h"
#include "OTGui/FillPainter2D.h"

#define OT_JSON_MEMBER_Size "Size"
#define OT_JSON_MEMBER_Outline "Outline"
#define OT_JSON_MEMBER_Direction "Direction"
#define OT_JSON_MEMBER_BackgroundPainter "BackgroundPainter"

static ot::SimpleFactoryRegistrar<ot::GraphicsTriangleItemCfg> triaItemCfg(OT_SimpleFactoryJsonKeyValue_GraphicsTriangleItemCfg);

std::string ot::GraphicsTriangleItemCfg::triangleDirectionToString(TriangleDirection _direction) {
	switch (_direction)
	{
	case ot::GraphicsTriangleItemCfg::Left: return "Left";
	case ot::GraphicsTriangleItemCfg::Up: return "Up";
	case ot::GraphicsTriangleItemCfg::Right: return "Right";
	case ot::GraphicsTriangleItemCfg::Down: return "Down";
	default:
		OT_LOG_WA("Unknown triangle direction");
		return "Right";
	}
}

ot::GraphicsTriangleItemCfg::TriangleDirection ot::GraphicsTriangleItemCfg::stringToTriangleDirection(const std::string& _str) {
	if (_str == triangleDirectionToString(ot::GraphicsTriangleItemCfg::Left)) return ot::GraphicsTriangleItemCfg::Left;
	else if (_str == triangleDirectionToString(ot::GraphicsTriangleItemCfg::Up)) return ot::GraphicsTriangleItemCfg::Up;
	else if (_str == triangleDirectionToString(ot::GraphicsTriangleItemCfg::Right)) return ot::GraphicsTriangleItemCfg::Right;
	else if (_str == triangleDirectionToString(ot::GraphicsTriangleItemCfg::Down)) return ot::GraphicsTriangleItemCfg::Down;
	else {
		OT_LOG_WAS("Unknown triangle direction \"" + _str + "\"");
		return ot::GraphicsTriangleItemCfg::Right;
	}
}

ot::GraphicsTriangleItemCfg::GraphicsTriangleItemCfg(TriangleDirection _direction)
	: m_direction(_direction)
{
	m_backgroundPainter = new ot::FillPainter2D(ot::Color(0, 0, 0, 0));
}

ot::GraphicsTriangleItemCfg::~GraphicsTriangleItemCfg() {
	if (m_backgroundPainter) delete m_backgroundPainter;
}

void ot::GraphicsTriangleItemCfg::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	OTAssertNullptr(m_backgroundPainter);

	GraphicsItemCfg::addToJsonObject(_document, _object);

	OT_rJSON_createValueObject(sizeObj);
	m_size.addToJsonObject(_document, sizeObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Size, sizeObj);

	OT_rJSON_createValueObject(backgroundPainterObj);
	m_backgroundPainter->addToJsonObject(_document, backgroundPainterObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_BackgroundPainter, backgroundPainterObj);

	OT_rJSON_createValueObject(outlineObj);
	m_outline.addToJsonObject(_document, outlineObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Outline, outlineObj);

	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Direction, this->triangleDirectionToString(m_direction));
}

void ot::GraphicsTriangleItemCfg::setFromJsonObject(OT_rJSON_val& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);

	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Direction, String);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Outline, Object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_BackgroundPainter, Object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Size, Object);

	m_direction = this->stringToTriangleDirection(_object[OT_JSON_MEMBER_Direction].GetString());

	OT_rJSON_val backgroundPainterObj = _object[OT_JSON_MEMBER_BackgroundPainter].GetObject();
	ot::Painter2D* p = ot::SimpleFactory::instance().createType<ot::Painter2D>(backgroundPainterObj);
	p->setFromJsonObject(backgroundPainterObj);
	this->setBackgroundPainer(p);

	OT_rJSON_val outlineObj = _object[OT_JSON_MEMBER_Outline].GetObject();
	m_outline.setFromJsonObject(outlineObj);

	OT_rJSON_val sizeObj = _object[OT_JSON_MEMBER_Size].GetObject();
	m_size.setFromJsonObject(sizeObj);
}

void ot::GraphicsTriangleItemCfg::setBackgroundPainer(ot::Painter2D* _painter) {
	if (_painter == m_backgroundPainter) return;
	if (m_backgroundPainter) delete m_backgroundPainter;
	m_backgroundPainter = _painter;
}
