//! @file dllmain.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2020
// ###########################################################################################################################################################################################################################################################################################################################

// SessionService header
#include "SessionService.h"
#include "GlobalSessionService.h"

// OpenTwin header
#include "OTSystem/AppExitCodes.h"
#include "OTCore/Logger.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ActionDispatcher.h"
#include "OTCommunication/ServiceLogNotifier.h"

// std header
#include <mutex>
#include <chrono>
#include <thread>
#include <fstream>
#include <cassert>
#include <iostream>
#include <exception>
#include <Windows.h>

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

static bool initDone{ false };

extern "C"
{
	// todo: switch from auth port to auth url
	_declspec(dllexport) int init(const char * _loggerServiceURL, const char * _ownUrl, const char * _globalSessionServiceURL, const char * _authPort) {
		std::string loggerServiceURL(_loggerServiceURL);

		// Initialize logging
#if defined(_DEBUG) || defined(OT_RELEASE_DEBUG)
		std::cout << "Local Session Service" << std::endl;
		ot::ServiceLogNotifier::initialize(OT_INFO_SERVICE_TYPE_SessionService, loggerServiceURL, true);
#else
		ot::ServiceLogNotifier::initialize(OT_INFO_SERVICE_TYPE_SessionService, loggerServiceURL, false);
#endif

		try {
			OT_LOG_I("Local Session Service initialization");

			// Now store the command line arguments and perform the initialization
			if (_ownUrl == nullptr) {
				exit(ot::AppExitCode::ServiceUrlInvalid);
			}
			if (_globalSessionServiceURL == nullptr) {
				exit(ot::AppExitCode::GSSUrlMissing);
			}
			if (_authPort == nullptr) {
				exit(ot::AppExitCode::AuthUrlMissing);
			}

			std::string ownUrl(_ownUrl);

			// Determine a good starting port for the services based on our own port number 
			size_t colonIndex = ownUrl.rfind(':');
			if (colonIndex == std::string::npos) {
				OT_LOG_EA("Unable to determine own port");
				exit(ot::AppExitCode::FailedToConvertPort);
			}

			std::string ip = ownUrl.substr(0, colonIndex);
			std::string port = ownUrl.substr(colonIndex + 1);
			
			// Register at GSS
			ot::JsonDocument gssDoc;
			gssDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_RegisterNewSessionService, gssDoc.GetAllocator()), gssDoc.GetAllocator());
			gssDoc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(ownUrl, gssDoc.GetAllocator()), gssDoc.GetAllocator());
			
			ot::JsonArray sessionArr;
			gssDoc.AddMember(OT_ACTION_PARAM_Sessions, sessionArr, gssDoc.GetAllocator());

			// Send request to GSS
			std::string gssResponse;

			// In case of error:
			// Minimum timeout: attempts * thread sleep                  = 30 * 500ms       =   15sec
			// Maximum timeout; attempts * (thread sleep + send timeout) = 30 * (500ms + 3s) = 1.45min
			const int maxCt = 30;
			int ct = 1;
			bool ok = false;
			do {
				gssResponse.clear();

				if (!(ok = ot::msg::send(ownUrl, _globalSessionServiceURL, ot::EXECUTE, gssDoc.toJson(), gssResponse, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit))) {
					OT_LOG_E("Register at Global Session Service failed [Attempt " + std::to_string(ct) + " / " + std::to_string(maxCt) + "]");
					using namespace std::chrono_literals;
					std::this_thread::sleep_for(500ms);
				}
			} while (!ok && ct++ <= maxCt);

			if (!ok) {
				OT_LOG_E("Registration at Global Session service failed after " + std::to_string(maxCt) + " attemts. Exiting...");
				exit(ot::AppExitCode::GSSRegistrationFailed);
			}

			ot::JsonDocument gssResponseDoc;
			gssResponseDoc.fromJson(gssResponse);

			ot::serviceID_t lssID = ot::json::getUInt(gssResponseDoc, OT_ACTION_PARAM_SERVICE_ID);
			std::string databaseURL = ot::json::getString(gssResponseDoc, OT_ACTION_PARAM_DATABASE_URL);
			std::string authURL = ot::json::getString(gssResponseDoc, OT_ACTION_PARAM_SERVICE_AUTHURL);
			std::string gdsURL;
			if (gssResponseDoc.HasMember(OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL)) {
				gdsURL = ot::json::getString(gssResponseDoc, OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL);
			}
			ot::LogModeManager logManager;
			if (gssResponseDoc.HasMember(OT_ACTION_PARAM_GlobalLogFlags)) {
				ot::ConstJsonArray logFlags = ot::json::getArray(gssResponseDoc, OT_ACTION_PARAM_GlobalLogFlags);
				logManager.setGlobalLogFlags(ot::logFlagsFromJsonArray(logFlags));
			}
			
			// Create session service and add data to session service
			SessionService& lss = SessionService::instance();
			lss.setDataBaseURL(databaseURL);
			lss.setIp(ip);
			lss.setPort(port);
			lss.setId(lssID);
			lss.setAuthorisationServiceURL(authURL);
			lss.updateLogMode(logManager);

			// Initialize GSS
			OT_LOG_D("Initializing GlobalSessionService...");
			GlobalSessionService * gss = new GlobalSessionService(_globalSessionServiceURL);
			lss.setGlobalSessionService(gss);
			gss->startHealthCheck();

			// Connect to GDS
			if (!gdsURL.empty()) {
				lss.setGlobalDirectoryServiceURL(gdsURL);
			}

			initDone = true;
			OT_LOG_D("Initialization finished");
			return ot::AppExitCode::Success;
		}
		catch (std::exception& e) {
			OT_LOG_EAS(std::string{"Uncaught exception: "}.append(e.what()));
			exit(ot::AppExitCode::GeneralError);
		}
		catch (...) {
			assert(0);
			exit(ot::AppExitCode::UnknownError);
		}
	};

	_declspec(dllexport) const char *performAction(const char * _json, const char * _senderIP) {
		return ot::ActionDispatcher::instance().dispatchWrapper(_json, _senderIP, ot::EXECUTE);
	};

	_declspec(dllexport) const char *performActionOneWayTLS(const char * _json, const char * _senderIP) {
		return ot::ActionDispatcher::instance().dispatchWrapper(_json, _senderIP, ot::EXECUTE_ONE_WAY_TLS);
	};

	// The session service is not queueing actions
	_declspec(dllexport) const char *queueAction(const char *_json, const char *_senderIP) {
		return ot::ActionDispatcher::instance().dispatchWrapper(_json, _senderIP, ot::QUEUE);
	};

	_declspec(dllexport) const char *getServiceURL(void) {
		assert(initDone);	// Init function not called
		std::string serviceURL = SessionService::instance().getUrl();

		char * retVal = new char[serviceURL.length() + 1];
		strcpy_s(retVal, serviceURL.length() + 1, serviceURL.c_str());
		return retVal;
	}

	_declspec(dllexport) void deallocateData(const char *data) {
		assert(initDone);	// Init function not called
		// Destroy data (return value form performAction and queueAction functions)
		if (data != nullptr) {
			delete[] data;
		}
	};
}
