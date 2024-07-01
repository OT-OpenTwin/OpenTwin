//! @file GraphicsEllipseItemCfg.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsEllipseItemCfg.h"
#include "OTGui/GraphicsItemCfgFactory.h"
#include "OTGui/Painter2D.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/Painter2DFactory.h"

#define OT_JSON_MEMBER_Outline "Outline"
#define OT_JSON_MEMBER_RadiusX "RadiusX"
#define OT_JSON_MEMBER_RadiusY "RadiusY"
#define OT_JSON_MEMBER_BackgroundPainter "BackgroundPainter"

static ot::GraphicsItemCfgFactoryRegistrar<ot::GraphicsEllipseItemCfg> ellipseItemRegistrar(OT_FactoryKey_GraphicsEllipseItem);

ot::GraphicsEllipseItemCfg::GraphicsEllipseItemCfg(double _radiusX, double _radiusY, ot::Painter2D* _backgroundPainter)
	: m_backgroundPainter(_backgroundPainter), m_radiusX(_radiusX), m_radiusY(_radiusY)
{
	if (m_backgroundPainter == nullptr) {
		// If no background painter was provided, we set a transparent background as default
		m_backgroundPainter = new ot::FillPainter2D(ot::Color(0, 0, 0, 0));
	}
}

ot::GraphicsEllipseItemCfg::GraphicsEllipseItemCfg(const GraphicsEllipseItemCfg& _other) 
	: GraphicsItemCfg(_other), m_radiusX(_other.m_radiusX), m_radiusY(_other.m_radiusY), m_outline(_other.m_outline), m_backgroundPainter(nullptr)
{
	m_backgroundPainter = _other.m_backgroundPainter->createCopy();
}

ot::GraphicsEllipseItemCfg::~GraphicsEllipseItemCfg() {
	if (m_backgroundPainter) delete m_backgroundPainter;
}

ot::GraphicsItemCfg* ot::GraphicsEllipseItemCfg::createCopy(void) const {
	return new GraphicsEllipseItemCfg(*this);
}

void ot::GraphicsEllipseItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	OTAssertNullptr(m_backgroundPainter);
	GraphicsItemCfg::addToJsonObject(_object, _allocator);

	_object.AddMember(OT_JSON_MEMBER_RadiusX, m_radiusX, _allocator);
	_object.AddMember(OT_JSON_MEMBER_RadiusY, m_radiusY, _allocator);

	JsonObject backgroundPainterObj;
	m_backgroundPainter->addToJsonObject(backgroundPainterObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_BackgroundPainter, backgroundPainterObj, _allocator);

	JsonObject outlineObj;
	m_outline.addToJsonObject(outlineObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_Outline, outlineObj, _allocator);
}

void ot::GraphicsEllipseItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);

	m_radiusX = json::getDouble(_object, OT_JSON_MEMBER_RadiusX);
	m_radiusY = json::getDouble(_object, OT_JSON_MEMBER_RadiusY);

	m_outline.setFromJsonObject(json::getObject(_object, OT_JSON_MEMBER_Outline));
	
	ConstJsonObject backgroundPainterObj = json::getObject(_object, OT_JSON_MEMBER_BackgroundPainter);
	ot::Painter2D* p = Painter2DFactory::instance().create(backgroundPainterObj);
	p->setFromJsonObject(backgroundPainterObj);
	this->setBackgroundPainer(p);
}

void ot::GraphicsEllipseItemCfg::setBackgroundPainer(ot::Painter2D* _painter) {
	if (_painter == m_backgroundPainter) return;
	if (m_backgroundPainter) delete m_backgroundPainter;
	m_backgroundPainter = _painter;
}
