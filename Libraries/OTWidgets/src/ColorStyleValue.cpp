//! @file ColorStyleValue.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/SimpleFactory.h"
#include "OTGui/Painter2D.h"
#include "OTGui/FillPainter2D.h"
#include "OTWidgets/ColorStyleValue.h"
#include "OTWidgets/Painter2DFactory.h"

ot::ColorStyleValue::ColorStyleValue()
	: m_painter(nullptr) 
{
	m_painter = new FillPainter2D;
}

ot::ColorStyleValue::ColorStyleValue(const ColorStyleValue& _other)
	: m_name(_other.m_name), m_qss(_other.m_qss), m_color(_other.m_color), m_brush(_other.m_brush), m_painter(nullptr)
{
	m_painter = _other.m_painter->createCopy();
}

ot::ColorStyleValue::~ColorStyleValue() {

}

ot::ColorStyleValue& ot::ColorStyleValue::operator = (const ColorStyleValue& _other) {
	m_name = _other.m_name;
	m_qss = _other.m_qss;
	m_color = _other.m_color;
	m_brush = _other.m_brush;
	m_painter = _other.m_painter->createCopy();
	
	return *this;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

void ot::ColorStyleValue::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_object.AddMember("QSS", JsonString(m_qss.toStdString(), _allocator), _allocator);

	JsonObject colorObj;
	colorObj.AddMember("A", m_color.alpha(), _allocator);
	colorObj.AddMember("R", m_color.red(), _allocator);
	colorObj.AddMember("G", m_color.green(), _allocator);
	colorObj.AddMember("B", m_color.blue(), _allocator);
	_object.AddMember("Color", colorObj, _allocator);

	JsonObject painterObj;
	m_painter->addToJsonObject(painterObj, _allocator);
	_object.AddMember("Painter", painterObj, _allocator);
}

void ot::ColorStyleValue::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_name = json::getString(_object, "Name");
	m_qss = QString::fromStdString(json::getString(_object, "QSS"));

	ConstJsonObject colorObj = json::getObject(_object, "Color");
	m_color.setAlpha(json::getInt(colorObj, "A"));
	m_color.setRed(json::getInt(colorObj, "R"));
	m_color.setGreen(json::getInt(colorObj, "G"));
	m_color.setBlue(json::getInt(colorObj, "B"));

	ConstJsonObject painterObj = json::getObject(_object, "Painter");
	Painter2D* newPainter = ot::SimpleFactory::instance().createType<Painter2D>(painterObj);
	if (!newPainter) {
		OT_LOG_E("Failed to create painter");
		return;
	}
	this->setPainter(newPainter);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter/Getter

void ot::ColorStyleValue::setPainter(Painter2D* _painter) {
	if (m_painter == _painter) return;
	if (m_painter) {
		delete m_painter;
	}
	m_painter = _painter;
	m_brush = Painter2DFactory::brushFromPainter2D(m_painter);
}