/*
 * dllmain.cpp
 *
 *  Created on: January 11, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2020 openTwin
 */

#include "AppBase.h"

// C++ header
#include <Windows.h>
#include <string>

#include "OpenTwinCommunication/ActionTypes.h"
#include "OpenTwinCore/Logger.h"
#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCommunication/IpConverter.h"
#include "OpenTwinCommunication/Msg.h"

// SessionService header

#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG


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

static std::string globalServiceURL;

std::string dispatchAction(const char * _json, const char * _senderIP) {
	try {
		OT_rJSON_parseDOC(actionDocument, _json);
		if (actionDocument.IsObject()) {

			std::string action = ot::rJSON::getString(actionDocument, OT_ACTION_MEMBER);

			return AppBase::instance().dispatchAction(action, actionDocument);
		}
		else {
			return OT_ACTION_RETURN_INDICATOR_Error "Ivalid action format";
		}
	}
	catch (const std::exception& _e) {
		return OT_ACTION_RETURN_INDICATOR_Error + std::string(_e.what());
	}
	catch (...) {
		return OT_ACTION_RETURN_UnknownError;
	}
}

extern "C"
{
	_declspec(dllexport) int init(const char * _unused1, const char * _serviceURL, const char * _unused2, const char * _unused3)
	{
		try {
			if (_serviceURL == nullptr) { return -1; }

			globalServiceURL = _serviceURL;
			
#ifdef _DEBUG
			ot::LogDispatcher::initialize(OT_INFO_SERVICE_TYPE_LOGGER, true);
			ot::LogDispatcher::instance().setLogFlags(ot::DEFAULT_LOG | ot::DETAILED_LOG | ot::WARNING_LOG | ot::ERROR_LOG);
			OT_LOG_D("LoggerService starting");
#else
			ot::LogDispatcher::initialize(OT_INFO_SERVICE_TYPE_LOGGER, false);
			ot::LogDispatcher::instance().setLogFlags(ot::NO_LOG);
#endif // _DEBUG
		}
		catch (const std::exception & e) {
			OutputDebugStringA("ERROR: ");
			OutputDebugStringA(e.what());
			OutputDebugStringA("\n");
			return 1;
		}
		catch (...) {
			OutputDebugStringA("ERROR: Unknown error");
			return 2;
		}
		return 0;
	};

	_declspec(dllexport) const char *getServiceURL(void)
	{
		char * retVal = new char[globalServiceURL.length() + 1];
		strcpy_s(retVal, globalServiceURL.length() + 1, globalServiceURL.c_str());
		return retVal;
	};

	_declspec(dllexport) const char *performAction(const char * _json, const char * _senderIP)
	{
		std::string ret = dispatchAction(_json, _senderIP);
		char *retval = new char[ret.length() + 1];
		strcpy_s(retval, ret.length() + 1, ret.c_str());
		return retval;
	};

	_declspec(dllexport) const char *performActionOneWayTLS(const char * _json, const char * _senderIP)
	{
		return performAction(_json, _senderIP);
	};

	// The logger service is not queueing actions
	_declspec(dllexport) const char *queueAction(const char *json, const char *senderIP)
	{
		return performAction(json, senderIP);
	};


	_declspec(dllexport) void deallocateData(const char *data)
	{
		// Destroy data (return value form performAction and queueAction functions)
		if (data != nullptr) delete[] data;
	};
}
