// @otlicense

// OpenTwin header
#include "OTGui/GraphicsLayoutItemCfg.h"

ot::GraphicsLayoutItemCfg::GraphicsLayoutItemCfg(const GraphicsLayoutItemCfg& _other) : GraphicsItemCfg(_other) {}

ot::GraphicsLayoutItemCfg::~GraphicsLayoutItemCfg() {}

void ot::GraphicsLayoutItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	GraphicsItemCfg::addToJsonObject(_object, _allocator);
}

void ot::GraphicsLayoutItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);
}
