//! @file Border.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/Border.h"
#include "OpenTwinCore/rJSONHelper.h"
#include "OpenTwinCore/otAssert.h"

ot::Border::Border() : m_color(0, 0, 0), m_width(1) {}

ot::Border::Border(const ot::Color& _color, int _width) : m_color(_color), m_width(_width) {}

ot::Border::Border(const Border& _other) : m_color(_other.m_color), m_width(_other.m_width) {}

ot::Border::~Border() {}

ot::Border& ot::Border::operator = (const Border& _other) {
	m_color = _other.m_color;
	m_width = _other.m_width;
	return *this;
}

bool ot::Border::operator == (const Border& _other) const {
	return m_color == _other.m_color && m_width == _other.m_width;
}

bool ot::Border::operator != (const Border& _other) const {
	return !(*this == _other);
}

void ot::Border::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _jsonObject) const {
	OT_rJSON_createValueObject(colorObj);
	m_color.addToJsonObject(_document, colorObj);
	ot::rJSON::add(_document, _jsonObject, "Color", colorObj);
	ot::rJSON::add(_document, _jsonObject, "Width", m_width);
}

void ot::Border::setFromJsonObject(OT_rJSON_val& _jsonObject) {
	OT_rJSON_checkMember(_jsonObject, "Color", Object);
	OT_rJSON_checkMember(_jsonObject, "Width", Int);
	OT_rJSON_val colorObj = _jsonObject["Color"].GetObject();
	m_color.setFromJsonObject(colorObj);
	m_width = _jsonObject["Width"].GetInt();
}