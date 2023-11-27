//! @file dllmain.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2022
// ###########################################################################################################################################################################################################################################################################################################################

// C++ header
#include <Windows.h>
#include <thread>
#include <cassert>
#include <exception>
#include <iostream>
#include <fstream>

// SessionService header
#include "GlobalSessionService.h"

// OpenTwin header
#include "OTCore/OTAssert.h"
#include "OTCore/Logger.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ServiceLogNotifier.h"
#include "OTServiceFoundation/Dispatcher.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

// #####################################################################################################################################

// Microservice Calls

extern "C"
{
	_declspec(dllexport) int init(const char * _loggerServiceURL, const char * _ownIP, const char * _databaseIP, const char * _authURL)
	{
		if (GlobalSessionService::hasInstance()) {
			OTAssert(0, "Global session service was already initialized");
			return -1;
		}

		// Now store the command line arguments and perform the initialization
		if (_ownIP == nullptr) { return -1; }
		if (_databaseIP == nullptr) { return -2; }
		if (_authURL == nullptr) { return -3; }

		std::string loggerServiceURL(_loggerServiceURL);

		// Initialize logging
#ifdef _DEBUG
		std::cout << "Global Session Service" << std::endl;
		ot::ServiceLogNotifier::initialize(OT_INFO_SERVICE_TYPE_GlobalSessionService, loggerServiceURL, true);
#else
		ot::ServiceLogNotifier::initialize(OT_INFO_SERVICE_TYPE_GlobalSessionService, loggerServiceURL, false);
#endif

		// Create session service and add data to session service
		GlobalSessionService * sessionService = GlobalSessionService::instance();
		sessionService->setDatabaseUrl(_databaseIP);
		sessionService->setUrl(_ownIP);
		sessionService->setAuthorizationUrl(_authURL);

		OT_LOG_I("Service initialized");

		return 0;
	}

	_declspec(dllexport) const char *performAction(const char * _json, const char * _senderIP)
	{
		return ot::Dispatcher::instance()->dispatchWrapper(_json, _senderIP, ot::EXECUTE);
	}

	_declspec(dllexport) const char *performActionOneWayTLS(const char * _json, const char * _senderIP) {
		return ot::Dispatcher::instance()->dispatchWrapper(_json, _senderIP, ot::EXECUTE_ONE_WAY_TLS);
	}

	// The session service is not queueing actions -> forward to execute
	_declspec(dllexport) const char *queueAction(const char *_json, const char *_senderIP) {
		return ot::Dispatcher::instance()->dispatchWrapper(_json, _senderIP, ot::QUEUE);
	};

	_declspec(dllexport) const char *getServiceURL(void)
	{
		char * returnValue = nullptr;

		// Check if the global session service was initialized
		if (!GlobalSessionService::hasInstance()) {
			OTAssert(0, "Global session service not initialized");
			returnValue = new char[1]{ 0 };
			return returnValue;
		}

		std::string serviceURL = GlobalSessionService::instance()->url();

		char * retVal = new char[serviceURL.length() + 1];
		strcpy_s(retVal, serviceURL.length() + 1, serviceURL.c_str());
		return retVal;
	}

	_declspec(dllexport) void deallocateData(const char *data)
	{
		// Destroy data (return value form performAction and queueAction functions)
		if (data) delete[] data;
	}
}
