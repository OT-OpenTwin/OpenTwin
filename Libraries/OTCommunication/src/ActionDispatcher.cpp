//! @file ActionDispatcher.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCommunication/ActionDispatcher.h"

ot::ActionDispatcher& ot::ActionDispatcher::instance(void) {
	static ActionDispatcher g_instance;
	return g_instance;
}
