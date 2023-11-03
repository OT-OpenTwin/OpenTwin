/*
 * dllmain.h
 *
 *	Author: Alexander Kuester
 *  Copyright (c) 2020 openTwin
 */

// App header
#include "AppBase.h"
#include "ExternalServicesComponent.h"

// OT header
#include "OpenTwinCommunication/ActionTypes.h"
#include "OpenTwinCommunication/ServiceLogNotifier.h"
#include "OpenTwinCore/Logger.h"

// C++ header
#include <exception>

int main(int argc, char *argv[])
{
	try {
		QByteArray loggingenv = qgetenv("OPEN_TWIN_LOGGING_URL");

		// Initialize logging
		ot::ServiceLogNotifier::initialize(OT_INFO_SERVICE_TYPE_UI, loggingenv.toStdString(), false);

		AppBase * app = AppBase::instance();
		app->setSiteID(0);
		app->getExternalServicesComponent()->setRelayServiceIsRequired();
		
		int status = app->run();
		ExternalServicesComponent * extComp = app->getExternalServicesComponent();
		if (extComp != nullptr) { extComp->shutdown(); }

		return status;
	}
	catch (const std::exception & e) {
		assert(0); // Something went wrong
		return -601;
	}
	catch (...) {
		assert(0);	// Something went horribly wrong
		return -602;
	}
}