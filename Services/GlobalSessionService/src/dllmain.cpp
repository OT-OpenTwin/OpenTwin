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
#include "OTSystem/AppExitCodes.h"
#include "OTCore/Logger.h"
#include "OTCore/OTAssert.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionDispatcher.h"
#include "OTCommunication/ServiceLogNotifier.h"

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
	_declspec(dllexport) int init(const char * _loggerServiceURL, const char * _ownIP, const char * _databaseIP, const char * _authURL) {
		// Initialize logging
		std::string loggerServiceURL(_loggerServiceURL);

#if defined(_DEBUG) || defined(OT_RELEASE_DEBUG)
		std::cout << "Global Session Service" << std::endl;
		ot::ServiceLogNotifier::initialize(OT_INFO_SERVICE_TYPE_GlobalSessionService, loggerServiceURL, true);
#else
		ot::ServiceLogNotifier::initialize(OT_INFO_SERVICE_TYPE_GlobalSessionService, loggerServiceURL, false);
#endif

		// Ensure arguments were passed
		if (_ownIP == nullptr) {
			OT_LOG_EA("Service URL not provided");
			exit(ot::AppExitCode::ServiceUrlMissing);
		}
		if (_databaseIP == nullptr) {
			OT_LOG_EA("Data Base URL not provided");
			exit(ot::AppExitCode::DataBaseUrlMissing);
		}
		if (_authURL == nullptr) {
			OT_LOG_EA("Authorization Service URL not provided");
			exit(ot::AppExitCode::AuthUrlMissing);
		}


		// Create session service and add data to session service
		GlobalSessionService& gss = GlobalSessionService::instance();
		gss.setDatabaseUrl(_databaseIP);
		gss.setUrl(_ownIP);
		gss.setAuthorizationUrl(_authURL);

		// Ensure data is somewhat valid
		if (gss.getUrl().empty()) {
			OT_LOG_EA("Empty Service URL provided");
			exit(ot::AppExitCode::ServiceUrlInvalid);
		}
		if (gss.getDatabaseUrl().empty()) {
			OT_LOG_EA("Empty Data Base URL provided");
			exit(ot::AppExitCode::DataBaseUrlInvalid);
		}
		if (gss.getAuthorizationUrl().empty()) {
			OT_LOG_EA("Empty Authorization Service URL provided");
			exit(ot::AppExitCode::AuthUrlInvalid);
		}

		OT_LOG_I("Service initialized");

		return ot::AppExitCode::Success;
	}

	_declspec(dllexport) const char *performAction(const char * _json, const char * _senderIP)
	{
		return ot::ActionDispatcher::instance().dispatchWrapper(_json, _senderIP, ot::EXECUTE);
	}

	_declspec(dllexport) const char *performActionOneWayTLS(const char * _json, const char * _senderIP) {
		return ot::ActionDispatcher::instance().dispatchWrapper(_json, _senderIP, ot::EXECUTE_ONE_WAY_TLS);
	}

	// The session service is not queueing actions -> forward to execute
	_declspec(dllexport) const char *queueAction(const char *_json, const char *_senderIP) {
		return ot::ActionDispatcher::instance().dispatchWrapper(_json, _senderIP, ot::QUEUE);
	};

	_declspec(dllexport) const char *getServiceURL(void)
	{
		char * returnValue = nullptr;

		std::string serviceURL = GlobalSessionService::instance().getUrl();

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
