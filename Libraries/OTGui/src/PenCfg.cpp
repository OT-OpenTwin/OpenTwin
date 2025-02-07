//! @file PenCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/PenCfg.h"
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

ot::PenCfg::PenCfg()
	: m_width(1), m_painter(nullptr), m_style(SolidLine), m_cap(SquareCap), m_join(BevelJoin)
{
	m_painter = new FillPainter2D(Black);
}

ot::PenCfg::PenCfg(const Color& _color)
	: m_width(1), m_painter(nullptr), m_style(SolidLine), m_cap(SquareCap), m_join(BevelJoin)
{
	m_painter = new FillPainter2D(_color);
}

ot::PenCfg::PenCfg(Painter2D* _painter)
	: m_width(1), m_painter(_painter), m_style(SolidLine), m_cap(SquareCap), m_join(BevelJoin)
{
	if (!m_painter) {
		OT_LOG_W("Nullptr provided for painter. Defaulting");
		m_painter = new FillPainter2D(Black);
	}
}

ot::PenCfg::PenCfg(int _width, const Color& _color)
	: m_width(_width), m_painter(nullptr), m_style(SolidLine), m_cap(SquareCap), m_join(BevelJoin)
{
	m_painter = new FillPainter2D(_color);
}

ot::PenCfg::PenCfg(int _width, Painter2D* _painter)
	: m_width(_width), m_painter(_painter), m_style(SolidLine), m_cap(SquareCap), m_join(BevelJoin)
{
	if (!m_painter) {
		OT_LOG_W("Nullptr provided for painter. Defaulting");
		m_painter = new FillPainter2D(Black);
	}
}

ot::PenCfg::PenCfg(const PenCfg& _other)
	: m_width(_other.m_width), m_painter(nullptr), m_style(_other.m_style), m_cap(_other.m_cap), m_join(_other.m_join)
{
	this->setPainter(_other.m_painter->createCopy());
}

ot::PenCfg::~PenCfg() {
	if (m_painter) delete m_painter;
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

void ot::PenCfg::setColor(const Color& _color) {
	this->setPainter(new FillPainter2D(_color));
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

ot::Painter2D* ot::PenCfg::takePainter(void) {
	Painter2D* ret = m_painter;
	m_painter = new FillPainter2D(Black);
	return ret;
}

ot::PenFCfg ot::PenCfg::toPenFCfg(void) const {
	if (m_painter) {
		return PenFCfg((double)m_width, m_painter->createCopy());
	}
	else {
		return PenFCfg((double)m_width, Color());
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::PenFCfg::PenFCfg()
	: m_width(1.), m_painter(nullptr), m_style(SolidLine), m_cap(SquareCap), m_join(BevelJoin)
{
	m_painter = new FillPainter2D(Black);
}

ot::PenFCfg::PenFCfg(const Color& _color)
	: m_width(1.), m_painter(nullptr), m_style(SolidLine), m_cap(SquareCap), m_join(BevelJoin)
{
	m_painter = new FillPainter2D(_color);
}

ot::PenFCfg::PenFCfg(Painter2D* _painter)
	: m_width(1.), m_painter(_painter), m_style(SolidLine), m_cap(SquareCap), m_join(BevelJoin)
{
	if (!m_painter) {
		OT_LOG_W("Nullptr provided for painter. Defaulting");
		m_painter = new FillPainter2D(Black);
	}
}

ot::PenFCfg::PenFCfg(double _width, const Color& _color)
	: m_width(_width), m_painter(nullptr), m_style(SolidLine), m_cap(SquareCap), m_join(BevelJoin)
{
	m_painter = new FillPainter2D(_color);
}

ot::PenFCfg::PenFCfg(double _width, Painter2D* _painter)
	: m_width(_width), m_painter(_painter), m_style(SolidLine), m_cap(SquareCap), m_join(BevelJoin)
{
	if (!m_painter) {
		OT_LOG_W("Nullptr provided for painter. Defaulting");
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

void ot::PenFCfg::setColor(const Color& _color) {
	this->setPainter(new FillPainter2D(_color));
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

ot::Painter2D* ot::PenFCfg::takePainter(void) {
	Painter2D* ret = m_painter;
	m_painter = new FillPainter2D(Black);
	return ret;
}

ot::PenCfg ot::PenFCfg::toPenCfg(void) const {
	if (m_painter) {
		return PenCfg((int)m_width, m_painter->createCopy());
	}
	else {
		return PenCfg((int)m_width, Color());
	}
}