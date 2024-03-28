//! @file PropertyPainter2D.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTCore/SimpleFactory.h"
#include "OTGui/PropertyPainter2D.h"
#include "OTGui/Painter2D.h"
#include "OTGui/FillPainter2D.h"

ot::PropertyPainter2D::PropertyPainter2D(PropertyFlags _flags)
	: Property(_flags), m_painter(nullptr)
{
	m_painter = new FillPainter2D;
}

ot::PropertyPainter2D::PropertyPainter2D(Painter2D* _painter, PropertyFlags _flags)
	: Property(_flags), m_painter(_painter)
{
	if (!m_painter) m_painter = new FillPainter2D;
}

ot::PropertyPainter2D::PropertyPainter2D(const Painter2D* _painter, PropertyFlags _flags)
	: Property(_flags), m_painter(nullptr)
{
	if (_painter) m_painter = _painter->createCopy();
	else m_painter = new FillPainter2D;
}

ot::PropertyPainter2D::PropertyPainter2D(const std::string& _name, Painter2D* _painter, PropertyFlags _flags)
	: Property(_name, _flags), m_painter(_painter)
{
	if (!m_painter) m_painter = new FillPainter2D;
}

ot::PropertyPainter2D::PropertyPainter2D(const std::string& _name, const Painter2D* _painter, PropertyFlags _flags)
	: Property(_name, _flags), m_painter(nullptr)
{
	if (_painter) m_painter = _painter->createCopy();
	else m_painter = new FillPainter2D;
}

ot::PropertyPainter2D::~PropertyPainter2D() {
	if (m_painter) delete m_painter;
	m_painter = nullptr;
}

void ot::PropertyPainter2D::getPropertyData(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	OTAssertNullptr(m_painter);

	JsonObject painterObj;
	m_painter->addToJsonObject(painterObj, _allocator);
	_object.AddMember("Value", painterObj, _allocator);
}

void ot::PropertyPainter2D::setPropertyData(const ot::ConstJsonObject& _object) {
	ConstJsonObject painterObj = json::getObject(_object, "Value");
	Painter2D* newPainter = SimpleFactory::instance().createType<Painter2D>(painterObj);
	if (newPainter) {
		this->setPainter(newPainter);
	}
}

void ot::PropertyPainter2D::setPainter(Painter2D* _painter) {
	if (m_painter == _painter) return;
	OTAssertNullptr(_painter);
	delete m_painter;
	m_painter = _painter;
}