// @otlicense

// OpenTwin header
#include "OTWidgets/GlobalWidgetViewManager.h"

ot::GlobalWidgetViewManager& ot::GlobalWidgetViewManager::instance() {
	static GlobalWidgetViewManager g_instance;
	return g_instance;
}