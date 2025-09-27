//! @file dllmain.cpp
//! @author Sebastian Urmann
//! @date May 2025
// ###########################################################################################################################################################################################################################################################################################################################

// LMS header
#include "Application.h"

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTCommunication/ActionDispatcher.h"
#include "OTCommunication/ServiceLogNotifier.h"

// std header
#include <iostream>
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

extern "C" {

	_declspec(dllexport) const char *performAction(const char * _json, const char * _senderURL)
	{
		OT_LOG("Execute called from \"" + std::string(_senderURL) + "\": \"" + _json + "\"", ot::INBOUND_MESSAGE_LOG);
		return ot::ActionDispatcher::instance().dispatchWrapper(_json, _senderURL, ot::EXECUTE);
	};

	_declspec(dllexport) const char *queueAction(const char * _json, const char * _senderURL)
	{
		OT_LOG("Queue called from \"" + std::string(_senderURL) + "\": \"" + _json + "\"", ot::QUEUED_INBOUND_MESSAGE_LOG);
		return ot::ActionDispatcher::instance().dispatchWrapper(_json, _senderURL, ot::QUEUE);
	};

	_declspec(dllexport) const char *getServiceURL(void)
	{
		std::string serviceURL = Application::instance().getServiceURL();

		char *strServiceURL = new char[serviceURL.size() + 1];
		strcpy_s(strServiceURL, serviceURL.size() + 1, serviceURL.c_str());

		return strServiceURL;
	}

	_declspec(dllexport) void deallocateData(const char * _data)
	{
		// *****************
		// This code will deallocate the memory of the return values from the perform- and queueAction calls
		if (_data != nullptr)
		{
			delete[] _data;
		}
		// *****************
	};

	// This function is called once upon startup of this service
	_declspec(dllexport) int init(const char * _loggerServiceURL, const char * _ownURL, const char * _globalSessionServiceURL, const char * _databasePWD) {
		std::string loggerServiceURL(_loggerServiceURL);

		Application::instance().setServiceURL(_ownURL);

#if defined(_DEBUG) || defined(OT_RELEASE_DEBUG)
		std::cout << "Library Management Service" << std::endl;
		ot::ServiceLogNotifier::initialize(OT_INFO_SERVICE_TYPE_LibraryManagementService, loggerServiceURL, true);
#else
		ot::ServiceLogNotifier::initialize(OT_INFO_SERVICE_TYPE_LibraryManagementService, loggerServiceURL, false);
#endif
		Application::instance().initialize("1", _ownURL, _globalSessionServiceURL, _databasePWD);
		return 0;

	};
}