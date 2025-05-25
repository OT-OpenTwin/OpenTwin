//! @file dllmain.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

// LDS header
#include "Application.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTServiceFoundation/Foundation.h"
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
	_declspec(dllexport) int init(const char * _loggerServiceURL, const char * _ownURL, const char * _globalDirectoryServiceURL, const char * _unused2) {
		std::string loggerServiceURL(_loggerServiceURL);

#if defined(_DEBUG) || defined(OT_RELEASE_DEBUG)
		std::cout << "Local Directory Service" << std::endl;
		ot::ServiceLogNotifier::initialize(OT_INFO_SERVICE_TYPE_LocalDirectoryService, loggerServiceURL, true);
#else
		ot::ServiceLogNotifier::initialize(OT_INFO_SERVICE_TYPE_LocalDirectoryService, loggerServiceURL, false);
#endif
		return Application::instance().initialize("127.0.0.1:9101", _globalDirectoryServiceURL);
	};
}