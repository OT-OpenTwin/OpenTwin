//! @file PropertyPainter2D.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/LogDispatcher.h"
#include "OTGui/Painter2D.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/Painter2DFactory.h"
#include "OTGui/PropertyPainter2D.h"
#include "OTGui/PropertyFactoryRegistrar.h"

static ot::PropertyFactoryRegistrar<ot::PropertyPainter2D> propertyPainter2DRegistrar(ot::PropertyPainter2D::propertyTypeString());

ot::PropertyPainter2D::PropertyPainter2D(const PropertyPainter2D* _other)
	: Property(_other), m_painter(nullptr)
{
	OTAssertNullptr(_other->m_painter);
	this->setPainter(_other->m_painter->createCopy());
	m_filter = _other->m_filter;
}

ot::PropertyPainter2D::PropertyPainter2D(const PropertyBase& _base)
	: Property(_base), m_painter(nullptr)
{
	m_painter = new FillPainter2D;
}

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

void ot::PropertyPainter2D::mergeWith(const Property* _other, const MergeMode& _mergeMode) {
	Property::mergeWith(_other, _mergeMode);

	if (_mergeMode & PropertyBase::MergeValues) {
		const PropertyPainter2D* other = dynamic_cast<const PropertyPainter2D*>(_other);
		OTAssertNullptr(other);
		this->setPainter(other->getPainter()->createCopy());
	}
}

ot::Property* ot::PropertyPainter2D::createCopy(void) const {
	return new PropertyPainter2D(this);
}

void ot::PropertyPainter2D::getPropertyData(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	OTAssertNullptr(m_painter);

	JsonObject painterObj;
	m_painter->addToJsonObject(painterObj, _allocator);
	_object.AddMember("Value", painterObj, _allocator);

	JsonObject filterObj;
	m_filter.addToJsonObject(filterObj, _allocator);
	_object.AddMember("Filter", filterObj, _allocator);
}

void ot::PropertyPainter2D::setPropertyData(const ot::ConstJsonObject& _object) {
	ConstJsonObject painterObj = json::getObject(_object, "Value");
	Painter2D* newPainter = Painter2DFactory::create(painterObj);
	if (newPainter) {
		this->setPainter(newPainter);
	}

	m_filter.setFromJsonObject(json::getObject(_object, "Filter"));
}

void ot::PropertyPainter2D::setPainter(Painter2D* _painter) {
	if (m_painter == _painter) return;
	OTAssertNullptr(_painter);
	if (m_painter) delete m_painter;
	m_painter = _painter;
}
