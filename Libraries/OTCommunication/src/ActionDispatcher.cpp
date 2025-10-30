// @otlicense

// OpenTwin header
#include "OTCommunication/ActionDispatcher.h"

ot::ActionDispatcher& ot::ActionDispatcher::instance(void) {
	static ActionDispatcher g_instance;
	return g_instance;
}
