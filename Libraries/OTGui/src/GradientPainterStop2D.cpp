// @otlicense

//! @file GradientPainterStop2D.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
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

void ot::GradientPainterStop2D::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember(OT_JSON_MEMBER_Pos, m_pos, _allocator);

	JsonObject colorObj;
	m_color.addToJsonObject(colorObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_Color, colorObj, _allocator);
}

void ot::GradientPainterStop2D::setFromJsonObject(const ConstJsonObject& _object) {
	m_pos = json::getDouble(_object, OT_JSON_MEMBER_Pos);
	m_color.setFromJsonObject(json::getObject(_object, OT_JSON_MEMBER_Color));
}
