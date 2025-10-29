// @otlicense

//! @file Border.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/Border.h"

ot::Border::Border(const PenCfg& _pen)
	: m_top(_pen), m_left(_pen), m_right(_pen), m_bottom(_pen)
{}

ot::Border::Border(const ot::Color& _color, int _width)
	: m_top(_width, _color), m_left(_width, _color), m_right(_width, _color), m_bottom(_width, _color)
{}

ot::Border::Border(const ot::Color& _color, int _leftWidth, int _topWidth, int _rightWidth, int _bottomWidth) 
	: m_top(_topWidth, _color), m_left(_leftWidth, _color), m_right(_rightWidth, _color), m_bottom(_bottomWidth, _color) 
{}

ot::Border::Border(const PenCfg& _left, const PenCfg& _top, const PenCfg& _right, const PenCfg& _bottom)
	: m_top(_top), m_left(_left), m_right(_right), m_bottom(_bottom)
{}

ot::Border::Border(const Border& _other)
	: m_top(_other.m_top), m_left(_other.m_left), m_right(_other.m_right), m_bottom(_other.m_bottom) {}

ot::Border::~Border() {}

ot::Border& ot::Border::operator = (const Border& _other) {
	m_top = _other.m_top;
	m_left = _other.m_left;
	m_right = _other.m_right;
	m_bottom = _other.m_bottom;
	return *this;
}

void ot::Border::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	JsonObject t;
	JsonObject l;
	JsonObject r;
	JsonObject b;

	m_top.addToJsonObject(t, _allocator);
	m_left.addToJsonObject(l, _allocator);
	m_right.addToJsonObject(r, _allocator);
	m_bottom.addToJsonObject(b, _allocator);

	_object.AddMember("Top", t, _allocator);
	_object.AddMember("Left", l, _allocator);
	_object.AddMember("Right", r, _allocator);
	_object.AddMember("Bottom", b, _allocator);
}

void ot::Border::setFromJsonObject(const ConstJsonObject& _object) {
	m_top.setFromJsonObject(json::getObject(_object, "Top"));
	m_left.setFromJsonObject(json::getObject(_object, "Left"));
	m_right.setFromJsonObject(json::getObject(_object, "Right"));
	m_bottom.setFromJsonObject(json::getObject(_object, "Bottom"));
}

void ot::Border::setColor(const ot::Color& _color) {
	m_top.setColor(_color);
	m_left.setColor(_color);
	m_right.setColor(_color);
	m_bottom.setColor(_color);
}

void ot::Border::setWidth(int _width) {
	m_top.setWidth(_width);
	m_left.setWidth(_width);
	m_right.setWidth(_width);
	m_bottom.setWidth(_width);
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::BorderF::BorderF(const ot::Color& _color, double _width)
	: m_top(_width, _color), m_left(_width, _color), m_right(_width, _color), m_bottom(_width, _color)
{}

ot::BorderF::BorderF(const ot::Color& _color, double _leftWidth, double _topWidth, double _rightWidth, double _bottomWidth)
	: m_top(_topWidth, _color), m_left(_leftWidth, _color), m_right(_rightWidth, _color), m_bottom(_bottomWidth, _color)
{}

ot::BorderF::BorderF(const PenFCfg& _left, const PenFCfg& _top, const PenFCfg& _right, const PenFCfg& _bottom)
	: m_top(_top), m_left(_left), m_right(_right), m_bottom(_bottom)
{}

ot::BorderF::BorderF(const BorderF& _other)
	: m_top(_other.m_top), m_left(_other.m_left), m_right(_other.m_right), m_bottom(_other.m_bottom) {}

ot::BorderF::~BorderF() {}

ot::BorderF& ot::BorderF::operator = (const BorderF& _other) {
	m_top = _other.m_top;
	m_left = _other.m_left;
	m_right = _other.m_right;
	m_bottom = _other.m_bottom;
	return *this;
}

void ot::BorderF::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	JsonObject t;
	JsonObject l;
	JsonObject r;
	JsonObject b;

	m_top.addToJsonObject(t, _allocator);
	m_left.addToJsonObject(l, _allocator);
	m_right.addToJsonObject(r, _allocator);
	m_bottom.addToJsonObject(b, _allocator);

	_object.AddMember("Top", t, _allocator);
	_object.AddMember("Left", l, _allocator);
	_object.AddMember("Right", r, _allocator);
	_object.AddMember("Bottom", b, _allocator);
}

void ot::BorderF::setFromJsonObject(const ConstJsonObject& _object) {
	m_top.setFromJsonObject(json::getObject(_object, "Top"));
	m_left.setFromJsonObject(json::getObject(_object, "Left"));
	m_right.setFromJsonObject(json::getObject(_object, "Right"));
	m_bottom.setFromJsonObject(json::getObject(_object, "Bottom"));
}

void ot::BorderF::setColor(const ot::Color& _color) {
	m_top.setColor(_color);
	m_left.setColor(_color);
	m_right.setColor(_color);
	m_bottom.setColor(_color);
}

void ot::BorderF::setWidth(double _width) {
	m_top.setWidth(_width);
	m_left.setWidth(_width);
	m_right.setWidth(_width);
	m_bottom.setWidth(_width);
}