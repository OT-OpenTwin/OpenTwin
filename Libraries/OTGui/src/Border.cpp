//! @file Border.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/Border.h"
#include "OTCore/rJSONHelper.h"

ot::Border::Border() : m_color(0, 0, 0), m_top(0), m_left(0), m_right(0), m_bottom(0) {}

ot::Border::Border(const ot::Color& _color, int _width) : m_color(_color), m_top(_width), m_left(_width), m_right(_width), m_bottom(_width) {}

ot::Border::Border(const ot::Color& _color, int _topWidth, int _leftWidth, int _rightWidth, int _bottomWidth) : m_color(_color), m_top(_topWidth), m_left(_leftWidth), m_right(_rightWidth), m_bottom(_bottomWidth) {}

ot::Border::Border(const Border& _other) : m_color(_other.m_color), m_top(_other.m_top), m_left(_other.m_left), m_right(_other.m_right), m_bottom(_other.m_bottom) {}

ot::Border::~Border() {}

ot::Border& ot::Border::operator = (const Border& _other) {
	m_color = _other.m_color;
	m_top = _other.m_top;
	m_left = _other.m_left;
	m_right = _other.m_right;
	m_bottom = _other.m_bottom;
	return *this;
}

bool ot::Border::operator == (const Border& _other) const {
	return m_color == _other.m_color && m_top == _other.m_top && m_left == _other.m_left && m_right == _other.m_right && m_bottom == _other.m_bottom;
}

bool ot::Border::operator != (const Border& _other) const {
	return !(*this == _other);
}

void ot::Border::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _jsonObject) const {
	OT_rJSON_createValueObject(colorObj);
	m_color.addToJsonObject(_document, colorObj);
	ot::rJSON::add(_document, _jsonObject, "Color", colorObj);
	ot::rJSON::add(_document, _jsonObject, "Top", m_top);
	ot::rJSON::add(_document, _jsonObject, "Left", m_left);
	ot::rJSON::add(_document, _jsonObject, "Right", m_right);
	ot::rJSON::add(_document, _jsonObject, "Bottom", m_bottom);
}

void ot::Border::setFromJsonObject(OT_rJSON_val& _jsonObject) {
	OT_rJSON_checkMember(_jsonObject, "Color", Object);
	OT_rJSON_checkMember(_jsonObject, "Top", Int);
	OT_rJSON_checkMember(_jsonObject, "Left", Int);
	OT_rJSON_checkMember(_jsonObject, "Right", Int);
	OT_rJSON_checkMember(_jsonObject, "Bottom", Int);
	
	OT_rJSON_val colorObj = _jsonObject["Color"].GetObject();
	m_color.setFromJsonObject(colorObj);
	
	m_top = _jsonObject["Top"].GetInt();
	m_left = _jsonObject["Left"].GetInt();
	m_right = _jsonObject["Right"].GetInt();
	m_bottom = _jsonObject["Bottom"].GetInt();
}

void ot::Border::setWidth(int _width) {
	m_top = _width;
	m_left = _width;
	m_right = _width;
	m_bottom = _width;
}