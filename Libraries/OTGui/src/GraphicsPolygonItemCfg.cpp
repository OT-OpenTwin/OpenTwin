//! @file GraphicsPolygonItemCfg.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsPolygonItemCfg.h"
#include "OTGui/Painter2D.h"
#include "OTGui/FillPainter2D.h"

static ot::SimpleFactoryRegistrar<ot::GraphicsPolygonItemCfg> polyItemCfg(OT_SimpleFactoryJsonKeyValue_GraphicsPolygonItemCfg);

ot::GraphicsPolygonItemCfg::GraphicsPolygonItemCfg()
{}

ot::GraphicsPolygonItemCfg::~GraphicsPolygonItemCfg() {
	
}

void ot::GraphicsPolygonItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	GraphicsItemCfg::addToJsonObject(_object, _allocator);

}

void ot::GraphicsPolygonItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);

}
