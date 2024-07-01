//! @file GraphicsArcItemCfg.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/Painter2D.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/Painter2DFactory.h"
#include "OTGui/GraphicsArcItemCfg.h"
#include "OTGui/GraphicsItemCfgFactory.h"

static ot::GraphicsItemCfgFactoryRegistrar<ot::GraphicsArcItemCfg> arcItemCfg(OT_FactoryKey_GraphicsArcItem);

ot::GraphicsArcItemCfg::GraphicsArcItemCfg()
	: m_startAngle(0.), m_spanAngle(180. * 16.)
{}

ot::GraphicsArcItemCfg::GraphicsArcItemCfg(const GraphicsArcItemCfg& _other)
	: GraphicsItemCfg(_other), m_rect(_other.m_rect), m_startAngle(_other.m_startAngle), m_spanAngle(_other.m_spanAngle), m_lineStyle(_other.m_lineStyle)
{}

ot::GraphicsArcItemCfg::~GraphicsArcItemCfg() {}

ot::GraphicsItemCfg* ot::GraphicsArcItemCfg::createCopy(void) const {
	return new GraphicsArcItemCfg(*this);
}

void ot::GraphicsArcItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	GraphicsItemCfg::addToJsonObject(_object, _allocator);

	JsonObject rectObj;
	m_rect.addToJsonObject(rectObj, _allocator);
	_object.AddMember("Rect", rectObj, _allocator);

	_object.AddMember("Start", m_startAngle, _allocator);
	_object.AddMember("Span", m_spanAngle, _allocator);

	JsonObject lineObj;
	m_lineStyle.addToJsonObject(lineObj, _allocator);
	_object.AddMember("LineStyle", lineObj, _allocator);
}

void ot::GraphicsArcItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);

	m_rect.setFromJsonObject(json::getObject(_object, "Rect"));
	m_startAngle = json::getDouble(_object, "Start");
	m_spanAngle = json::getDouble(_object, "Span");
	m_lineStyle.setFromJsonObject(json::getObject(_object, "LineStyle"));
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter
