//! @file GradientPainterStop2D.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/rJSONHelper.h"
#include "OTGui/GradientPainterStop2D.h"

#define OT_JSON_MEMBER_Pos "Pos"
#define OT_JSON_MEMBER_Color "Color"

ot::GradientPainterStop2D::GradientPainterStop2D()
	: m_pos(0.) {}

ot::GradientPainterStop2D::GradientPainterStop2D(double _pos, const ot::Color& _color)
	: m_pos(_pos), m_color(_color) {}

ot::GradientPainterStop2D::GradientPainterStop2D(const GradientPainterStop2D& _other)
	: m_pos(_other.m_pos), m_color(_other.m_color) {}

ot::GradientPainterStop2D::~GradientPainterStop2D() {}

ot::GradientPainterStop2D& ot::GradientPainterStop2D::operator = (const GradientPainterStop2D& _other) {
	m_pos = _other.m_pos;
	m_color = _other.m_color;

	return *this;
}

bool ot::GradientPainterStop2D::operator == (const GradientPainterStop2D& _other) const {
	return m_pos == _other.m_pos && m_color == _other.m_color;
}

bool ot::GradientPainterStop2D::operator != (const GradientPainterStop2D& _other) const {
	return m_pos == _other.m_pos || m_color != _other.m_color;
}

void ot::GradientPainterStop2D::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Pos, m_pos);
	OT_rJSON_createValueObject(colorObj);
	m_color.addToJsonObject(_document, colorObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Color, colorObj);
}

void ot::GradientPainterStop2D::setFromJsonObject(OT_rJSON_val& _object) {
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Pos, Double);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Color, Object);
	m_pos = _object[OT_JSON_MEMBER_Pos].GetDouble();
	OT_rJSON_val colorObj = _object[OT_JSON_MEMBER_Color].GetObject();
	m_color.setFromJsonObject(colorObj);
}
