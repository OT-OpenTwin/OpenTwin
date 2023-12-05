/*
 * dllmain.cpp
 *
 *  Created on: November 26, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2020 openTwin
 */

// C++ header
#include <Windows.h>
#include <thread>
#include <cassert>
#include <exception>
#include <iostream>
#include <fstream>
#include <mutex>

// SessionService header
#include "globalDatatypes.h"
#include "SessionService.h"
#include "GlobalSessionService.h"

#include "OTCore/Logger.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ServiceLogNotifier.h"
#include "OTServiceFoundation/Dispatcher.h"

std::ofstream *logfile = nullptr;

void logMessage(const std::string &msg)
{
	if (logfile == nullptr) return;

	*logfile << msg << std::endl;
	logfile->flush();
}

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
	_declspec(dllexport) int init(const char * _loggerServiceURL, const char * _ownUrl, const char * _globalSessionServiceURL, const char * _authPort)
	{
		std::string loggerServiceURL(_loggerServiceURL);

		// Initialize logging
#ifdef _DEBUG
		std::cout << "Local Session Service" << std::endl;
		ot::ServiceLogNotifier::initialize(OT_INFO_SERVICE_TYPE_SessionService, loggerServiceURL, true);
#else
		ot::ServiceLogNotifier::initialize(OT_INFO_SERVICE_TYPE_SessionService, loggerServiceURL, false);
#endif

		try {
			OT_LOG_I("Local Session Service initialization");

			// Now store the command line arguments and perform the initialization
			if (_ownUrl == nullptr) { return -1; }
			if (_globalSessionServiceURL == nullptr) { return -2; }
			if (_authPort == nullptr) { return -3; }

			std::string ownUrl(_ownUrl);

			// Determine a good starting port for the services based on our own port number 
			size_t colonIndex = ownUrl.rfind(':');
			if (colonIndex == std::string::npos)
			{
				OT_LOG_E("Unable to determine own port");
				assert(0);
				return 1;
			}

			std::string ip = ownUrl.substr(0, colonIndex);
			std::string port = ownUrl.substr(colonIndex + 1);
			
#ifdef OT_USE_GSS

			// Register at GSS
			ot::JsonDocument gssDoc;
			gssDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_RegisterNewSessionService, gssDoc.GetAllocator()), gssDoc.GetAllocator());
			gssDoc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(ownUrl, gssDoc.GetAllocator()), gssDoc.GetAllocator());
			
			ot::JsonArray sessionArr;
			gssDoc.AddMember(OT_ACTION_PARAM_SESSION_LIST, sessionArr, gssDoc.GetAllocator());

			// Send request to GSS
			std::string gssResponse;
			if (!ot::msg::send(ownUrl, _globalSessionServiceURL, ot::EXECUTE, gssDoc.toJson(), gssResponse)) {
				OT_LOG_E("Failed to send request to GlobalSessionService");
				return 20;
			}

			ot::JsonDocument gssResponseDoc;
			gssResponseDoc.fromJson(gssResponse);

			ot::serviceID_t ssID = ot::json::getUInt(gssResponseDoc, OT_ACTION_PARAM_SERVICE_ID);
			std::string databaseURL = ot::json::getString(gssResponseDoc, OT_ACTION_PARAM_DATABASE_URL);
			std::string authURL = ot::json::getString(gssResponseDoc, OT_ACTION_PARAM_SERVICE_AUTHURL);
			std::string gdsURL;
			if (gssResponseDoc.HasMember(OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL)) {
				gdsURL = ot::json::getString(gssResponseDoc, OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL);
			}

			// Create session service and add data to session service
			SessionService * sessionService = SessionService::instance();
			sessionService->setDataBaseURL(databaseURL);
			sessionService->setIp(ip);
			sessionService->setPort(port);
			sessionService->setId(ssID);
			sessionService->setAuthorisationServiceURL(authURL);

			// Initialize GSS
			OT_LOG_D("Initializing GlobalSessionService...");
			GlobalSessionService * gss = new GlobalSessionService(_globalSessionServiceURL, sessionService);
			sessionService->setGlobalSessionService(gss);
			gss->startHealthCheck();

			// Connect to GDS
			if (!gdsURL.empty()) sessionService->setGlobalDirectoryServiceURL(gdsURL);
#else
			// Create session service and add data to session service
			SessionService * sessionService = SessionService::instance();
			sessionService->setDataBaseIP(_globalSessionServiceURL);
			sessionService->setURL(_ownIP);
#endif

#ifndef OT_USE_GSS
			std::string authorizationServiceIP = sessionService->globalServiceIP() + ":" + _authPort;
			sessionService->setAuthorisationServiceIP(authorizationServiceIP);
#endif // !OT_USE_GSS

			initDone = true;
			OT_LOG_D("Initialization finished");
			return 0;
		}
		catch (std::exception& e) {
			OT_LOG_E(std::string{"Uncaught exception: "}.append(e.what()));
			assert(0);
			return 1;
		}
		catch (...) {
			assert(0);
			return 1;
		}
	};

	_declspec(dllexport) const char *performAction(const char * _json, const char * _senderIP)
	{
		return ot::Dispatcher::instance()->dispatchWrapper(_json, _senderIP, ot::EXECUTE);
	};

	_declspec(dllexport) const char *performActionOneWayTLS(const char * _json, const char * _senderIP)
	{
		return ot::Dispatcher::instance()->dispatchWrapper(_json, _senderIP, ot::EXECUTE_ONE_WAY_TLS);
	};

	// The session service is not queueing actions
	_declspec(dllexport) const char *queueAction(const char *_json, const char *_senderIP)
	{
		return ot::Dispatcher::instance()->dispatchWrapper(_json, _senderIP, ot::QUEUE);
	};

	_declspec(dllexport) const char *getServiceURL(void)
	{
		assert(initDone);	// Init function not called
		std::string serviceURL = SessionService::instance()->url();

		char * retVal = new char[serviceURL.length() + 1];
		strcpy_s(retVal, serviceURL.length() + 1, serviceURL.c_str());
		return retVal;
	}

	_declspec(dllexport) void deallocateData(const char *data)
	{
		assert(initDone);	// Init function not called
		// Destroy data (return value form performAction and queueAction functions)
		if (data != nullptr)
		{ delete[] data; }
	};
}
