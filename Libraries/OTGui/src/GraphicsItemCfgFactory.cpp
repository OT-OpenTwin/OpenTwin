// @otlicense

// OpenTwin header
#include "OTGui/GraphicsItemCfgFactory.h"

ot::GraphicsItemCfgFactory& ot::GraphicsItemCfgFactory::instance(void) {
	static GraphicsItemCfgFactory g_instance;
	return g_instance;
}

ot::GraphicsItemCfg* ot::GraphicsItemCfgFactory::create(const ConstJsonObject& _object) {
	return GraphicsItemCfgFactory::instance().createFromJSON(_object, GraphicsItemCfg::factoryTypeKey());
}
