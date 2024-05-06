//! @file ColorStyleValue.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/Painter2D.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/Painter2DFactory.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/ColorStyleValue.h"

ot::ColorStyleValue::ColorStyleValue()
	: m_painter(nullptr) 
{
	m_painter = new FillPainter2D;
}

ot::ColorStyleValue::ColorStyleValue(const ColorStyleValue& _other)
	: m_name(_other.m_name), m_painter(nullptr)
{
	m_painter = _other.m_painter->createCopy();
}

ot::ColorStyleValue::~ColorStyleValue() {

}

ot::ColorStyleValue& ot::ColorStyleValue::operator = (const ColorStyleValue& _other) {
	m_name = _other.m_name;
	m_painter = _other.m_painter->createCopy();
	
	return *this;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

void ot::ColorStyleValue::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Name", JsonString(m_name, _allocator), _allocator);

	JsonObject painterObj;
	m_painter->addToJsonObject(painterObj, _allocator);
	_object.AddMember("Painter", painterObj, _allocator);
}

void ot::ColorStyleValue::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_name = json::getString(_object, "Name");

	ConstJsonObject painterObj = json::getObject(_object, "Painter");
	Painter2D* newPainter = Painter2DFactory::instance().create(painterObj);
	if (!newPainter) {
		OT_LOG_E("Failed to create painter");
		return;
	}
	newPainter->setFromJsonObject(painterObj);
	this->setPainter(newPainter);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter/Getter

QString ot::ColorStyleValue::qss(void) const {
	OTAssertNullptr(m_painter);
	if (m_painter) return QString::fromStdString(m_painter->generateQss());
	else return "";
}

QColor ot::ColorStyleValue::color(void) const {
	OTAssertNullptr(m_painter);
	if (m_painter) return QtFactory::toQt(m_painter->getDefaultColor());
	else return QColor();
}

QBrush ot::ColorStyleValue::brush(void) const {
	return QtFactory::toQt(m_painter);
}

void ot::ColorStyleValue::setPainter(Painter2D* _painter) {
	if (m_painter == _painter) return;
	if (m_painter) {
		delete m_painter;
	}
	m_painter = _painter;
}

void ot::ColorStyleValue::setPainter(const Painter2D* _painter) {
	this->setPainter(_painter->createCopy());
}

void ot::ColorStyleValue::setColor(Color _color) {
	this->setPainter(new FillPainter2D(_color));
}

void ot::ColorStyleValue::setColor(DefaultColor _color) {
	this->setPainter(new FillPainter2D(_color));
}