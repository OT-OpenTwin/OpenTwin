// @otlicense
// File: PenCfg.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/PenCfg.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/Painter2DFactory.h"
#include "OTGui/StyleRefPainter2D.h"

std::string ot::toString(LineStyle _style) {
	switch (_style)
	{
	case LineStyle::NoLine: return "None";
	case LineStyle::SolidLine: return "Solid";
	case LineStyle::DashLine: return "Dashed";
	case LineStyle::DotLine: return "Dotted";
	case LineStyle::DashDotLine: return "Dash Dot";
	case LineStyle::DashDotDotLine: return "Dash Dot Dot";
	default:
		OT_LOG_E("Unknown line style (" + std::to_string((int)_style) + ")");
		return "Solid";
	}
}

ot::LineStyle ot::stringToLineStyle(const std::string& _style) {
	if (_style == toString(LineStyle::NoLine)) return LineStyle::NoLine;
	else if (_style == toString(LineStyle::SolidLine)) return LineStyle::SolidLine;
	else if (_style == toString(LineStyle::DashLine)) return LineStyle::DashLine;
	else if (_style == toString(LineStyle::DotLine)) return LineStyle::DotLine;
	else if (_style == toString(LineStyle::DashDotLine)) return LineStyle::DashDotLine;
	else if (_style == toString(LineStyle::DashDotDotLine)) return LineStyle::DashDotDotLine;
	else {
		OT_LOG_E("Unknown line style \"" + _style + "\"");
		return LineStyle::SolidLine;
	}
}

std::string ot::toString(LineCapStyle _cap) {
	switch (_cap)
	{
	case LineCapStyle::FlatCap: return "Flat";
	case LineCapStyle::SquareCap: return "Square";
	case LineCapStyle::RoundCap: return "Round";
	default:
		OT_LOG_E("Unknown cap style (" + std::to_string((int)_cap) + ")");
		return "Square";
	}
}

ot::LineCapStyle ot::stringToCapStyle(const std::string& _cap) {
	if (_cap == toString(LineCapStyle::FlatCap)) return LineCapStyle::FlatCap;
	else if (_cap == toString(LineCapStyle::SquareCap)) return LineCapStyle::SquareCap;
	else if (_cap == toString(LineCapStyle::RoundCap)) return LineCapStyle::RoundCap;
	else {
		OT_LOG_E("Unknown cap style \"" + _cap + "\"");
		return LineCapStyle::SquareCap;
	}
}

std::string ot::toString(LineJoinStyle _join) {
	switch (_join)
	{
	case ot::LineJoinStyle::MiterJoin: return "Milter";
	case ot::LineJoinStyle::BevelJoin: return "Bevel";
	case ot::LineJoinStyle::RoundJoin: return "Round";
	case ot::LineJoinStyle::SvgMiterJoin: return "SvgMiter";
	default:
		OT_LOG_E("Unknown join style (" + std::to_string((int)_join) + ")");
		return "Bevel";
	}
}

ot::LineJoinStyle ot::stringToJoinStyle(const std::string& _join) {
	if (_join == toString(LineJoinStyle::MiterJoin)) return LineJoinStyle::MiterJoin;
	else if (_join == toString(LineJoinStyle::BevelJoin)) return LineJoinStyle::BevelJoin;
	else if (_join == toString(LineJoinStyle::RoundJoin)) return LineJoinStyle::RoundJoin;
	else if (_join == toString(LineJoinStyle::SvgMiterJoin)) return LineJoinStyle::SvgMiterJoin;
	else {
		OT_LOG_E("Unknown join style \"" + _join + "\"");
		return LineJoinStyle::BevelJoin;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::PenCfg::PenCfg() : PenCfg(1, nullptr) {};

ot::PenCfg::PenCfg(DefaultColor _color) : PenCfg(1, new FillPainter2D(Color(_color))) {};

ot::PenCfg::PenCfg(const Color& _color) : PenCfg(1, new FillPainter2D(_color)) {};

ot::PenCfg::PenCfg(Painter2D* _painter) : PenCfg(1, _painter) {};

ot::PenCfg::PenCfg(int _width, DefaultColor _color) : PenCfg(_width, new FillPainter2D(Color(_color))) {};

ot::PenCfg::PenCfg(int _width, const Color& _color) : PenCfg(_width, new FillPainter2D(_color)) {};

ot::PenCfg::PenCfg(int _width, Painter2D* _painter)
	: m_width(_width), m_painter(_painter), m_style(LineStyle::SolidLine), m_cap(LineCapStyle::SquareCap), m_join(LineJoinStyle::BevelJoin)
{
	if (!m_painter) {
		m_painter = new FillPainter2D(Black);
	}
}

ot::PenCfg::PenCfg(const PenCfg& _other)
	: m_width(_other.m_width), m_painter(nullptr), m_style(_other.m_style), m_cap(_other.m_cap), m_join(_other.m_join)
{
	this->setPainter(_other.m_painter->createCopy());
}

ot::PenCfg::~PenCfg() {
	if (m_painter) 
	{
		delete m_painter;
	}
	m_painter = nullptr;
}

ot::PenCfg& ot::PenCfg::operator = (const PenCfg& _other) {
	if (this == &_other) return *this;
	m_width = _other.m_width;
	m_style = _other.m_style;
	m_cap = _other.m_cap;
	m_join = _other.m_join;

	this->setPainter(_other.m_painter->createCopy());

	return *this;
}

void ot::PenCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Width", m_width, _allocator);
	_object.AddMember("Style", JsonString(toString(m_style), _allocator), _allocator);
	_object.AddMember("Cap", JsonString(toString(m_cap), _allocator), _allocator);
	_object.AddMember("Join", JsonString(toString(m_join), _allocator), _allocator);

	JsonObject painterObj;
	m_painter->addToJsonObject(painterObj, _allocator);
	_object.AddMember("Painter", painterObj, _allocator);
}

void ot::PenCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_width = json::getInt(_object, "Width");
	m_style = stringToLineStyle(json::getString(_object, "Style"));
	m_cap = stringToCapStyle(json::getString(_object, "Cap"));
	m_join = stringToJoinStyle(json::getString(_object, "Join"));

	ConstJsonObject painterObj = json::getObject(_object, "Painter");
	Painter2D* p = Painter2DFactory::create(painterObj);
	if (p) {
		this->setPainter(p);
	}
}

void ot::PenCfg::setColor(DefaultColor _color) {
	this->setPainter(new FillPainter2D(_color));
}

void ot::PenCfg::setColor(const Color& _color) {
	this->setPainter(new FillPainter2D(_color));
}

void ot::PenCfg::setColor(ColorStyleValueEntry _color) {
	this->setPainter(new StyleRefPainter2D(_color));
}

void ot::PenCfg::setPainter(Painter2D* _painter) {
	if (m_painter == _painter) return;
	if (!_painter) {
		OT_LOG_W("Nullptr provided. Ignoring");
		return;
	}

	if (m_painter) delete m_painter;
	m_painter = _painter;
}

ot::Painter2D* ot::PenCfg::takePainter() {
	Painter2D* ret = m_painter;
	m_painter = new FillPainter2D(Black);
	return ret;
}

ot::PenFCfg ot::PenCfg::toPenFCfg() const {
	if (m_painter) {
		return PenFCfg(static_cast<double>(m_width), m_painter->createCopy());
	}
	else {
		return PenFCfg(static_cast<double>(m_width), Color());
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::PenFCfg::PenFCfg() : PenFCfg(1., nullptr) {}

ot::PenFCfg::PenFCfg(DefaultColor _color) : PenFCfg(1., new FillPainter2D(Color(_color))) {}

ot::PenFCfg::PenFCfg(const Color& _color) : PenFCfg(1., new FillPainter2D(_color)) {}

ot::PenFCfg::PenFCfg(Painter2D* _painter) : PenFCfg(1., _painter) {}

ot::PenFCfg::PenFCfg(double _width, DefaultColor _color) : PenFCfg(_width, new FillPainter2D(Color(_color))) {}

ot::PenFCfg::PenFCfg(double _width, const Color& _color) : PenFCfg(_width, new FillPainter2D(_color)) {}

ot::PenFCfg::PenFCfg(double _width, Painter2D* _painter) : 
	m_width(_width), m_painter(_painter), m_style(LineStyle::SolidLine), m_cap(LineCapStyle::SquareCap), m_join(LineJoinStyle::BevelJoin)
{
	if (!m_painter) {
		m_painter = new FillPainter2D(Black);
	}
}

ot::PenFCfg::PenFCfg(const PenFCfg& _other)
	: m_width(_other.m_width), m_painter(nullptr), m_style(_other.m_style), m_cap(_other.m_cap), m_join(_other.m_join)
{
	this->setPainter(_other.m_painter->createCopy());
}

ot::PenFCfg::~PenFCfg() {
	if (m_painter) delete m_painter;
	m_painter = nullptr;
}

ot::PenFCfg& ot::PenFCfg::operator = (const PenFCfg& _other) {
	if (this == &_other) return *this;
	m_width = _other.m_width;
	m_style = _other.m_style;
	m_cap = _other.m_cap;
	m_join = _other.m_join;

	this->setPainter(_other.m_painter->createCopy());

	return *this;
}

void ot::PenFCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Width", m_width, _allocator);
	_object.AddMember("Style", JsonString(toString(m_style), _allocator), _allocator);
	_object.AddMember("Cap", JsonString(toString(m_cap), _allocator), _allocator);
	_object.AddMember("Join", JsonString(toString(m_join), _allocator), _allocator);

	JsonObject painterObj;
	m_painter->addToJsonObject(painterObj, _allocator);
	_object.AddMember("Painter", painterObj, _allocator);
}

void ot::PenFCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_width = json::getDouble(_object, "Width");
	m_style = stringToLineStyle(json::getString(_object, "Style"));
	m_cap = stringToCapStyle(json::getString(_object, "Cap"));
	m_join = stringToJoinStyle(json::getString(_object, "Join"));
	
	ConstJsonObject painterObj = json::getObject(_object, "Painter");
	Painter2D* p = Painter2DFactory::create(painterObj);
	if (p) {
		this->setPainter(p);
	}
}

void ot::PenFCfg::setColor(DefaultColor _color) {
	this->setPainter(new FillPainter2D(_color));
}

void ot::PenFCfg::setColor(const Color& _color) {
	this->setPainter(new FillPainter2D(_color));
}

void ot::PenFCfg::setColor(ColorStyleValueEntry _color) {
	this->setPainter(new StyleRefPainter2D(_color));
}

void ot::PenFCfg::setPainter(Painter2D* _painter) {
	if (m_painter == _painter) return;
	if (!_painter) {
		OT_LOG_W("Nullptr provided. Ignoring");
		return;
	}

	if (m_painter) delete m_painter;
	m_painter = _painter;
}

ot::Painter2D* ot::PenFCfg::takePainter() {
	Painter2D* ret = m_painter;
	m_painter = new FillPainter2D(Black);
	return ret;
}

ot::PenCfg ot::PenFCfg::toPenCfg() const {
	if (m_painter) {
		return PenCfg(static_cast<int>(m_width), m_painter->createCopy());
	}
	else {
		return PenCfg(static_cast<int>(m_width), Color());
	}
}