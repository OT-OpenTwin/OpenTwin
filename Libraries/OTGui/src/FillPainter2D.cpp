#include "OTGui/FillPainter2D.h"

ot::FillPainter2D::FillPainter2D() {}

ot::FillPainter2D::FillPainter2D(const ot::Color& _color) : m_color(_color) {}

ot::FillPainter2D::FillPainter2D(const FillPainter2D& _other) : m_color(_other.m_color) {}

ot::FillPainter2D::~FillPainter2D() {}

ot::FillPainter2D& ot::FillPainter2D::operator = (const FillPainter2D& _other) {
	m_color = _other.m_color;
	return *this;
}

bool ot::FillPainter2D::operator == (const FillPainter2D& _other) const {
	return m_color == _other.m_color;
}

bool ot::FillPainter2D::operator != (const FillPainter2D& _other) const {
	return m_color != _other.m_color;
}

void ot::FillPainter2D::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::Painter2D::addToJsonObject(_document, _object);
	m_color.addToJsonObject(_document, _object);
}

void ot::FillPainter2D::setFromJsonObject(OT_rJSON_val& _object) {
	ot::Painter2D::setFromJsonObject(_object);
	m_color.setFromJsonObject(_object);
}