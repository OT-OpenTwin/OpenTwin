//! @file LineStyle.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/LineStyle.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/Painter2DFactory.h"

ot::LineStyle::LineStyle()
	: m_width(1.), m_painter(nullptr)
{
	m_painter = new FillPainter2D(Black);
}

ot::LineStyle::LineStyle(Painter2D* _painter) 
	: m_width(1.), m_painter(_painter)
{
	if (!m_painter) {
		OT_LOG_W("Nullptr provided for painter. Defaulting");
		m_painter = new FillPainter2D(Black);
	}
}

ot::LineStyle::LineStyle(double _width, Painter2D* _painter) 
	: m_width(_width), m_painter(_painter)
{
	if (!m_painter) {
		OT_LOG_W("Nullptr provided for painter. Defaulting");
		m_painter = new FillPainter2D(Black);
	}
}

ot::LineStyle::LineStyle(const LineStyle& _other) 
	: m_width(_other.m_width), m_painter(nullptr)
{
	this->setPainter(_other.m_painter);
}

ot::LineStyle::~LineStyle() {
	if (m_painter) delete m_painter;
	m_painter = nullptr;
}

ot::LineStyle& ot::LineStyle::operator = (const LineStyle& _other) {
	if (this == &_other) return *this;
	m_width = _other.m_width;

	this->setPainter(_other.m_painter);

	return *this;
}

void ot::LineStyle::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Width", m_width, _allocator);

	JsonObject painterObj;
	m_painter->addToJsonObject(painterObj, _allocator);
	_object.AddMember("Painter", painterObj, _allocator);
}

void ot::LineStyle::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_width = json::getDouble(_object, "Width");
	
	ConstJsonObject painterObj = json::getObject(_object, "Painter");
	Painter2D* p = Painter2DFactory::instance().create(painterObj);
	if (p) {
		this->setPainter(p);
	}
}

void ot::LineStyle::setPainter(Painter2D* _painter) {
	if (m_painter == _painter) return;
	if (!_painter) {
		OT_LOG_W("Nullptr provided. Ignoring");
		return;
	}

	if (m_painter) delete m_painter;
	m_painter = _painter;
}

ot::Painter2D* ot::LineStyle::takePainter(void) {
	Painter2D* ret = m_painter;
	m_painter = new FillPainter2D(Black);
	return ret;
}