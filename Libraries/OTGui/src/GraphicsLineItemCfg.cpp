//! @file GraphicsLineItemCfg.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/Painter2D.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/Painter2DFactory.h"
#include "OTGui/GraphicsLineItemCfg.h"
#include "OTGui/GraphicsItemCfgFactory.h"

static ot::GraphicsItemCfgFactoryRegistrar<ot::GraphicsLineItemCfg> lineItemCfg(OT_FactoryKey_GraphicsLineItem);

ot::GraphicsLineItemCfg::GraphicsLineItemCfg() {}

ot::GraphicsLineItemCfg::~GraphicsLineItemCfg() {}

ot::GraphicsItemCfg* ot::GraphicsLineItemCfg::createCopy(void) const {
	ot::GraphicsLineItemCfg* copy = new GraphicsLineItemCfg;
	this->setupData(copy);

	copy->m_from = m_from;
	copy->m_to = m_to;
	copy->m_lineStyle = m_lineStyle;

	return copy;
}

void ot::GraphicsLineItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	GraphicsItemCfg::addToJsonObject(_object, _allocator);

	JsonObject fromObj;
	m_from.addToJsonObject(fromObj, _allocator);
	_object.AddMember("From", fromObj, _allocator);

	JsonObject toObj;
	m_to.addToJsonObject(toObj, _allocator);
	_object.AddMember("To", toObj, _allocator);

	JsonObject lineObj;
	m_lineStyle.addToJsonObject(lineObj, _allocator);
	_object.AddMember("LineStyle", lineObj, _allocator);
}

void ot::GraphicsLineItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);

	m_from.setFromJsonObject(json::getObject(_object, "From"));
	m_to.setFromJsonObject(json::getObject(_object, "To"));
	m_lineStyle.setFromJsonObject(json::getObject(_object, "LineStyle"));
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter
