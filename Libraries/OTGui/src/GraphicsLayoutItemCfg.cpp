//! @file GraphcisLayoutItemCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsLayoutItemCfg.h"
#include "OTCore/rJSONHelper.h"

ot::GraphicsLayoutItemCfg::~GraphicsLayoutItemCfg() {
	
}

void ot::GraphicsLayoutItemCfg::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	GraphicsItemCfg::addToJsonObject(_document, _object);
}

void ot::GraphicsLayoutItemCfg::setFromJsonObject(OT_rJSON_val& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);
}
