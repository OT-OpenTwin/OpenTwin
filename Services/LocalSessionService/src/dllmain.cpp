// @otlicense

// SessionService header
#include "SessionService.h"
#include "GlobalSessionService.h"

// OpenTwin header
#include "OTSystem/AppExitCodes.h"
#include "OTCore/LogDispatcher.h"
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

extern "C"
{
	// todo: switch from auth port to auth url
	_declspec(dllexport) int init(const char* _loggerServiceURL, const char* _ownUrl, const char* _globalSessionServiceURL, const char* _unused) {
#ifdef _DEBUG
		std::cout << "Local Session Service" << std::endl;
#endif // _DEBUG

		try {
			// Now store the command line arguments and perform the initialization
			if (_ownUrl == nullptr) {
				exit(ot::AppExitCode::ServiceUrlInvalid);
			}
			if (_globalSessionServiceURL == nullptr) {
				exit(ot::AppExitCode::GSSUrlMissing);
			}

			std::string ownUrl(_ownUrl);

			// Create session service and add data to session service
			int iniResult = SessionService::initialize(_ownUrl, _globalSessionServiceURL);

			if (iniResult != ot::AppExitCode::Success) {
				exit(iniResult);
			}

			return iniResult;
		}
		catch (std::exception& e) {
			OT_LOG_EAS(std::string{"Uncaught exception: "}.append(e.what()));
			exit(ot::AppExitCode::GeneralError);
		}
		catch (...) {
			OTAssert(0, "Unknown error");
			exit(ot::AppExitCode::UnknownError);
		}
	};

	_declspec(dllexport) const char *performAction(const char* _json, const char* _senderUrl) {
		return ot::ActionDispatcher::instance().dispatchWrapper(_json, _senderUrl, ot::EXECUTE);
	};

	_declspec(dllexport) const char *performActionOneWayTLS(const char* _json, const char* _senderUrl) {
		return ot::ActionDispatcher::instance().dispatchWrapper(_json, _senderUrl, ot::EXECUTE_ONE_WAY_TLS);
	};

	// The session service is not queueing actions
	_declspec(dllexport) const char *queueAction(const char* _json, const char* _senderUrl) {
		return ot::ActionDispatcher::instance().dispatchWrapper(_json, _senderUrl, ot::QUEUE);
	};

	_declspec(dllexport) const char *getServiceURL() {
		std::string serviceURL = SessionService::instance().getUrl();

		char * retVal = new char[serviceURL.length() + 1];
		strcpy_s(retVal, serviceURL.length() + 1, serviceURL.c_str());
		return retVal;
	}

	_declspec(dllexport) void deallocateData(const char* _data) {
		if (_data != nullptr) {
			delete[] _data;
		}
	};
}
