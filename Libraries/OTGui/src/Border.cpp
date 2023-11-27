//! @file Border.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/Border.h"

ot::Border::Border() : m_color(0, 0, 0), m_top(0), m_left(0), m_right(0), m_bottom(0) {}

ot::Border::Border(const ot::Color& _color, int _width) : m_color(_color), m_top(_width), m_left(_width), m_right(_width), m_bottom(_width) {}

ot::Border::Border(const ot::Color& _color, int _leftWidth, int _topWidth, int _rightWidth, int _bottomWidth) : m_color(_color), m_top(_topWidth), m_left(_leftWidth), m_right(_rightWidth), m_bottom(_bottomWidth) {}

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

void ot::Border::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	JsonObject colorObj;
	m_color.addToJsonObject(colorObj, _allocator);

	_object.AddMember("Color", colorObj, _allocator);
	_object.AddMember("Left", m_left, _allocator);
	_object.AddMember("Top", m_top, _allocator);
	_object.AddMember("Right", m_right, _allocator);
	_object.AddMember("Bottom", m_bottom, _allocator);
}

void ot::Border::setFromJsonObject(const ConstJsonObject& _object) {
	m_color.setFromJsonObject(json::getObject(_object, "Color"));
	
	m_left = json::getInt(_object, "Left");
	m_top = json::getInt(_object, "Top");
	m_right = json::getInt(_object, "Right");
	m_bottom = json::getInt(_object, "Bottom");
}

void ot::Border::setWidth(int _width) {
	m_top = _width;
	m_left = _width;
	m_right = _width;
	m_bottom = _width;
}