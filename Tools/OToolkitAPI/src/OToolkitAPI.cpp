//! @file OToolkitAPI.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2023
// ###########################################################################################################################################################################################################################################################################################################################

#include "OToolkitAPI/OToolkitAPI.h"

#include "OpenTwinCore/otAssert.h"

namespace otoolkit {

	namespace intern {
		static otoolkit::APIInterface* getGlobalInterface(otoolkit::APIInterface* _interface) {
			static otoolkit::APIInterface* g_interface{ nullptr };
			if (g_interface == nullptr) {
				g_interface = _interface;
			}
			OTAssertNullptr(g_interface);
			otAssert(g_interface != _interface, "Global Interface already set");
			return g_interface;
		}
	}
}

// #################################################################################################################################################################################################################################################################################################################################

// #################################################################################################################################################################################################################################################################################################################################

// #################################################################################################################################################################################################################################################################################################################################

void otoolkit::api::initialize(APIInterface* _interface) {
	APIInterface* instance = otoolkit::intern::getGlobalInterface(_interface);
	if (instance != _interface) {
		return;
	}

	OTOOL_LOG("API", "Initialization done");
}

otoolkit::APIInterface* otoolkit::api::getGlobalInterface(void) {
	return otoolkit::intern::getGlobalInterface(nullptr);
}
