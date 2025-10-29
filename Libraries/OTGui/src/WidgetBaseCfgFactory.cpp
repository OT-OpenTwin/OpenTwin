// @otlicense

//! @file WidgetBaseCfgFactory.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/WidgetBaseCfgFactory.h"

ot::WidgetBaseCfgFactory& ot::WidgetBaseCfgFactory::instance(void) {
	static WidgetBaseCfgFactory g_instance;
	return g_instance;
}

ot::WidgetBaseCfg* ot::WidgetBaseCfgFactory::create(const ConstJsonObject& _object) {
	return WidgetBaseCfgFactory::instance().createFromJSON(_object, WidgetBaseCfg::typeStringJsonKey());
}
