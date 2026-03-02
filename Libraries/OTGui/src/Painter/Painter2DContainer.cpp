// @otlicense

// OpenTwin header
#include "OTGui/Painter/FillPainter2D.h"
#include "OTGui/Painter/Painter2DFactory.h"
#include "OTGui/Painter/Painter2DContainer.h"

ot::Painter2DContainer::Painter2DContainer() : m_painter(new FillPainter2D()) 
{}

ot::Painter2DContainer::Painter2DContainer(Painter2D* _initialPainter)
	: m_painter(_initialPainter)
{
	OTAssertNullptr(m_painter);
}

ot::Painter2DContainer::Painter2DContainer(const Painter2D* _initialPainter)
	: m_painter(_initialPainter->createCopy()) 
{
	OTAssertNullptr(m_painter);
}

ot::Painter2DContainer::Painter2DContainer(const Painter2DContainer& _other) 
	: m_painter(_other->createCopy())
{}

ot::Painter2DContainer::Painter2DContainer(Painter2DContainer&& _other) noexcept 
	: m_painter(_other.m_painter)
{
	OTAssertNullptr(m_painter);
	_other.m_painter = new FillPainter2D();
}

ot::Painter2DContainer::~Painter2DContainer() {
	OTAssertNullptr(m_painter);
	delete m_painter;
}

ot::Painter2DContainer& ot::Painter2DContainer::operator=(const Painter2DContainer& _other) {
	if (this != &_other) {
		delete m_painter;
		m_painter = _other->createCopy();
	}

	return *this;
}

ot::Painter2DContainer& ot::Painter2DContainer::operator=(Painter2DContainer&& _other) noexcept {
	if (this != &_other) {
		delete m_painter;
		m_painter = _other.m_painter;
		_other.m_painter = new FillPainter2D();
	}

	return *this;
}

void ot::Painter2DContainer::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	OTAssertNullptr(m_painter);
	m_painter->addToJsonObject(_object, _allocator);
}

void ot::Painter2DContainer::setFromJsonObject(const ConstJsonObject& _object) {
	setPainter(Painter2DFactory::create(_object));
}

void ot::Painter2DContainer::setPainter(Painter2D* _painter) {
	OTAssertNullptr(_painter);

	if (m_painter != _painter) {
		delete m_painter;
		m_painter = _painter;
	}
}
