//! @file GraphcisLayoutItemCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsLayoutItemCfg.h"

ot::GraphicsLayoutItemCfg::~GraphicsLayoutItemCfg() {
	
}

void ot::GraphicsLayoutItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	GraphicsItemCfg::addToJsonObject(_object, _allocator);
}

void ot::GraphicsLayoutItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);
}

void ot::GraphicsLayoutItemCfg::setupData(GraphicsItemCfg* _config) const {
	GraphicsItemCfg::setupData(_config);
}