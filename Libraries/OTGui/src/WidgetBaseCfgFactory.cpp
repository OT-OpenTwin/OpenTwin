// @otlicense

// OpenTwin header
#include "OTGui/WidgetBaseCfgFactory.h"

ot::WidgetBaseCfgFactory& ot::WidgetBaseCfgFactory::instance(void) {
	static WidgetBaseCfgFactory g_instance;
	return g_instance;
}

ot::WidgetBaseCfg* ot::WidgetBaseCfgFactory::create(const ConstJsonObject& _object) {
	return WidgetBaseCfgFactory::instance().createFromJSON(_object, WidgetBaseCfg::typeStringJsonKey());
}
