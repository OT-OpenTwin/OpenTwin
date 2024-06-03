//! @file Outline.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/Outline.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/Painter2DFactory.h"

std::string ot::toString(LineStyle _style) {
	switch (_style)
	{
	case ot::NoLine: return "NoLine";
	case ot::SolidLine: return "SolidLine";
	case ot::DashLine: return "DashLine";
	case ot::DotLine: return "DotLine";
	case ot::DashDotLine: return "DashDotLine";
	case ot::DashDotDotLine: return "DashDotDotLine";
	default:
		OT_LOG_E("Unknown line style (" + std::to_string((int)_style) + ")");
		return "NoLine";
	}
}

ot::LineStyle ot::stringToLineStyle(const std::string& _style) {
	if (_style == toString(NoLine)) return NoLine;
	else if (_style == toString(SolidLine)) return SolidLine;
	else if (_style == toString(DashLine)) return DashLine;
	else if (_style == toString(DotLine)) return DotLine;
	else if (_style == toString(DashDotLine)) return DashDotLine;
	else if (_style == toString(DashDotDotLine)) return DashDotDotLine;
	else {
		OT_LOG_E("Unknown line style \"" + _style + "\"");
		return NoLine;
	}
}

std::string ot::toString(LineCapStyle _cap) {
	switch (_cap)
	{
	case ot::FlatCap: return "Flat";
	case ot::SquareCap: return "Square";
	case ot::RoundCap: return "Round";
	default:
		OT_LOG_E("Unknown cap style (" + std::to_string((int)_cap) + ")");
		return "Square";
	}
}

ot::LineCapStyle ot::stringToCapStyle(const std::string& _cap) {
	if (_cap == toString(FlatCap)) return FlatCap;
	else if (_cap == toString(SquareCap)) return SquareCap;
	else if (_cap == toString(RoundCap)) return RoundCap;
	else {
		OT_LOG_E("Unknown cap style \"" + _cap + "\"");
		return SquareCap;
	}
}

std::string ot::toString(LineJoinStyle _join) {
	switch (_join)
	{
	case ot::MiterJoin: return "Milter";
	case ot::BevelJoin: return "Bevel";
	case ot::RoundJoin: return "Round";
	case ot::SvgMiterJoin: return "SvgMiter";
	default:
		OT_LOG_E("Unknown join style (" + std::to_string((int)_join) + ")");
		return "Bevel";
	}
}

ot::LineJoinStyle ot::stringToJoinStyle(const std::string& _join) {
	if (_join == toString(MiterJoin)) return MiterJoin;
	else if (_join == toString(BevelJoin)) return BevelJoin;
	else if (_join == toString(RoundJoin)) return RoundJoin;
	else if (_join == toString(SvgMiterJoin)) return SvgMiterJoin;
	else {
		OT_LOG_E("Unknown join style \"" + _join + "\"");
		return BevelJoin;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::Outline::Outline()
	: m_width(1), m_painter(nullptr), m_style(SolidLine), m_cap(SquareCap), m_join(BevelJoin)
{
	m_painter = new FillPainter2D(Black);
}

ot::Outline::Outline(const Color& _color)
	: m_width(1), m_painter(nullptr), m_style(SolidLine), m_cap(SquareCap), m_join(BevelJoin)
{
	m_painter = new FillPainter2D(_color);
}

ot::Outline::Outline(Painter2D* _painter)
	: m_width(1), m_painter(_painter), m_style(SolidLine), m_cap(SquareCap), m_join(BevelJoin)
{
	if (!m_painter) {
		OT_LOG_W("Nullptr provided for painter. Defaulting");
		m_painter = new FillPainter2D(Black);
	}
}

ot::Outline::Outline(int _width, const Color& _color)
	: m_width(_width), m_painter(nullptr), m_style(SolidLine), m_cap(SquareCap), m_join(BevelJoin)
{
	m_painter = new FillPainter2D(_color);
}

ot::Outline::Outline(int _width, Painter2D* _painter)
	: m_width(_width), m_painter(_painter), m_style(SolidLine), m_cap(SquareCap), m_join(BevelJoin)
{
	if (!m_painter) {
		OT_LOG_W("Nullptr provided for painter. Defaulting");
		m_painter = new FillPainter2D(Black);
	}
}

ot::Outline::Outline(const Outline& _other)
	: m_width(_other.m_width), m_painter(nullptr), m_style(_other.m_style), m_cap(_other.m_cap), m_join(_other.m_join)
{
	this->setPainter(_other.m_painter->createCopy());
}

ot::Outline::~Outline() {
	if (m_painter) delete m_painter;
	m_painter = nullptr;
}

ot::Outline& ot::Outline::operator = (const Outline& _other) {
	if (this == &_other) return *this;
	m_width = _other.m_width;
	m_style = _other.m_style;
	m_cap = _other.m_cap;
	m_join = _other.m_join;

	this->setPainter(_other.m_painter->createCopy());

	return *this;
}

void ot::Outline::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Width", m_width, _allocator);
	_object.AddMember("Style", JsonString(toString(m_style), _allocator), _allocator);
	_object.AddMember("Cap", JsonString(toString(m_cap), _allocator), _allocator);
	_object.AddMember("Join", JsonString(toString(m_join), _allocator), _allocator);

	JsonObject painterObj;
	m_painter->addToJsonObject(painterObj, _allocator);
	_object.AddMember("Painter", painterObj, _allocator);
}

void ot::Outline::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_width = json::getInt(_object, "Width");
	m_style = stringToLineStyle(json::getString(_object, "Style"));
	m_cap = stringToCapStyle(json::getString(_object, "Cap"));
	m_join = stringToJoinStyle(json::getString(_object, "Join"));

	ConstJsonObject painterObj = json::getObject(_object, "Painter");
	Painter2D* p = Painter2DFactory::instance().create(painterObj);
	if (p) {
		this->setPainter(p);
	}
}

void ot::Outline::setColor(const Color& _color) {
	this->setPainter(new FillPainter2D(_color));
}

void ot::Outline::setPainter(Painter2D* _painter) {
	if (m_painter == _painter) return;
	if (!_painter) {
		OT_LOG_W("Nullptr provided. Ignoring");
		return;
	}

	if (m_painter) delete m_painter;
	m_painter = _painter;
}

ot::Painter2D* ot::Outline::takePainter(void) {
	Painter2D* ret = m_painter;
	m_painter = new FillPainter2D(Black);
	return ret;
}

ot::OutlineF ot::Outline::toOutlineF(void) const {
	if (m_painter) {
		return OutlineF((double)m_width, m_painter->createCopy());
	}
	else {
		return OutlineF((double)m_width, Color());
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::OutlineF::OutlineF()
	: m_width(1.), m_painter(nullptr), m_style(SolidLine), m_cap(SquareCap), m_join(BevelJoin)
{
	m_painter = new FillPainter2D(Black);
}

ot::OutlineF::OutlineF(const Color& _color)
	: m_width(1.), m_painter(nullptr), m_style(SolidLine), m_cap(SquareCap), m_join(BevelJoin)
{
	m_painter = new FillPainter2D(_color);
}

ot::OutlineF::OutlineF(Painter2D* _painter)
	: m_width(1.), m_painter(_painter), m_style(SolidLine), m_cap(SquareCap), m_join(BevelJoin)
{
	if (!m_painter) {
		OT_LOG_W("Nullptr provided for painter. Defaulting");
		m_painter = new FillPainter2D(Black);
	}
}

ot::OutlineF::OutlineF(double _width, const Color& _color)
	: m_width(_width), m_painter(nullptr), m_style(SolidLine), m_cap(SquareCap), m_join(BevelJoin)
{
	m_painter = new FillPainter2D(_color);
}

ot::OutlineF::OutlineF(double _width, Painter2D* _painter)
	: m_width(_width), m_painter(_painter), m_style(SolidLine), m_cap(SquareCap), m_join(BevelJoin)
{
	if (!m_painter) {
		OT_LOG_W("Nullptr provided for painter. Defaulting");
		m_painter = new FillPainter2D(Black);
	}
}

ot::OutlineF::OutlineF(const OutlineF& _other)
	: m_width(_other.m_width), m_painter(nullptr), m_style(_other.m_style), m_cap(_other.m_cap), m_join(_other.m_join)
{
	this->setPainter(_other.m_painter->createCopy());
}

ot::OutlineF::~OutlineF() {
	if (m_painter) delete m_painter;
	m_painter = nullptr;
}

ot::OutlineF& ot::OutlineF::operator = (const OutlineF& _other) {
	if (this == &_other) return *this;
	m_width = _other.m_width;
	m_style = _other.m_style;
	m_cap = _other.m_cap;
	m_join = _other.m_join;

	this->setPainter(_other.m_painter->createCopy());

	return *this;
}

void ot::OutlineF::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Width", m_width, _allocator);
	_object.AddMember("Style", JsonString(toString(m_style), _allocator), _allocator);
	_object.AddMember("Cap", JsonString(toString(m_cap), _allocator), _allocator);
	_object.AddMember("Join", JsonString(toString(m_join), _allocator), _allocator);

	JsonObject painterObj;
	m_painter->addToJsonObject(painterObj, _allocator);
	_object.AddMember("Painter", painterObj, _allocator);
}

void ot::OutlineF::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_width = json::getDouble(_object, "Width");
	m_style = stringToLineStyle(json::getString(_object, "Style"));
	m_cap = stringToCapStyle(json::getString(_object, "Cap"));
	m_join = stringToJoinStyle(json::getString(_object, "Join"));
	
	ConstJsonObject painterObj = json::getObject(_object, "Painter");
	Painter2D* p = Painter2DFactory::instance().create(painterObj);
	if (p) {
		this->setPainter(p);
	}
}

void ot::OutlineF::setColor(const Color& _color) {
	this->setPainter(new FillPainter2D(_color));
}

void ot::OutlineF::setPainter(Painter2D* _painter) {
	if (m_painter == _painter) return;
	if (!_painter) {
		OT_LOG_W("Nullptr provided. Ignoring");
		return;
	}

	if (m_painter) delete m_painter;
	m_painter = _painter;
}

ot::Painter2D* ot::OutlineF::takePainter(void) {
	Painter2D* ret = m_painter;
	m_painter = new FillPainter2D(Black);
	return ret;
}

ot::Outline ot::OutlineF::toOutline(void) const {
	if (m_painter) {
		return Outline((int)m_width, m_painter->createCopy());
	}
	else {
		return Outline((int)m_width, Color());
	}
}