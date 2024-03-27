//! @file OToolkitAPI.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2023
// ###########################################################################################################################################################################################################################################################################################################################

#include "OToolkitAPI/OToolkitAPI.h"

// OpenTwin header
#include "OTCore/OTAssert.h"
#include "OTWidgets/IconManager.h"
#include "OTWidgets/GlobalColorStyle.h"

// Qt header
#include <QtCore/qdir.h>

namespace otoolkit {

	namespace intern {
		OTOOLKITAPI_EXPORTONLY otoolkit::APIInterface* getGlobalInterface(otoolkit::APIInterface* _interface) {
			static otoolkit::APIInterface* g_interface{ nullptr };
			if (g_interface == nullptr) {
				g_interface = _interface;
			}
			else {
				OTAssert(g_interface != _interface, "Global Interface already set");
			}
			OTAssertNullptr(g_interface);
			return g_interface;
		}
	}
}

// #################################################################################################################################################################################################################################################################################################################################

// #################################################################################################################################################################################################################################################################################################################################

// #################################################################################################################################################################################################################################################################################################################################

otoolkit::APIInterface::APIInterface() {

}

otoolkit::APIInterface::~APIInterface() {

}

// #################################################################################################################################################################################################################################################################################################################################

// #################################################################################################################################################################################################################################################################################################################################

// #################################################################################################################################################################################################################################################################################################################################

bool otoolkit::api::initialize(APIInterface* _interface) {
	APIInterface* instance = otoolkit::intern::getGlobalInterface(_interface);
	if (instance != _interface) {
		return false;
	}

	// Setup icon manager
	int iconPathCounter{ 0 };
	int stylePathCounter{ 0 };
#ifdef _DEBUG
	if (ot::IconManager::instance().addSearchPath(QString(qgetenv("OPENTWIN_DEV_ROOT") + "/Assets/Icons/"))) {
		iconPathCounter++;
	}
	if (ot::GlobalColorStyle::instance().addStyleRootSearchPath(QString(qgetenv("OPENTWIN_DEV_ROOT") + "/Assets/ColorStyles/"))) {
		stylePathCounter++;
	}
#endif // _DEBUG

	if (ot::IconManager::instance().addSearchPath(QDir::currentPath() + "/icons/")) {
		iconPathCounter++;
	}
	if (ot::GlobalColorStyle::instance().addStyleRootSearchPath(QDir::currentPath() + "/ColorStyles/")) {
		stylePathCounter++;
	}

	// Check if at least one icon directory was found
	if (iconPathCounter == 0) {
		OTAssert(0, "No icon path was found!");
		OT_LOG_E("No icon path found");
		return false;
	}

	// Check if at least one style directory was found
	if (stylePathCounter == 0) {
		OT_LOG_EA("No color style path found");
		return false;
	}

	return true;
}

otoolkit::APIInterface* otoolkit::api::getGlobalInterface(void) {
	return otoolkit::intern::getGlobalInterface(nullptr);
}
