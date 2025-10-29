// @otlicense

/*
 * dllmain.cpp
 */

// C++ header
#include <Windows.h>

#undef min
#undef max

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTCore/ServiceBase.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/ServiceLogNotifier.h"

// Service header
#include "ServiceBase.h"

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

extern "C"{

	_declspec(dllexport) const char *performAction(const char * _json, const char * _senderIP)
	{
		std::string response = ServiceBase::GetInstance().performAction(_json, _senderIP);

		// Copy the return value. The memory of this value will be deallocated in the deallocateData function
		char * retVal = new char[response.length() + 1];
		strcpy_s(retVal, response.length() + 1, response.c_str());
		return retVal;
	};

	_declspec(dllexport) const char *performActionOneWayTLS(const char * _json, const char * _senderIP)
	{
		std::string response = ServiceBase::GetInstance().performActionOneWayTLS(_json, _senderIP);

		// Copy the return value. The memory of this value will be deallocated in the deallocateData function
		char * retVal = new char[response.length() + 1];
		strcpy_s(retVal, response.length() + 1, response.c_str());
		return retVal;
	};

	_declspec(dllexport) const char *queueAction(const char * _json, const char * _senderIP)
	{
		// We are not queueing any messages here -> immediate processing
		std::string response = ServiceBase::GetInstance().performAction(_json, _senderIP);

		// Copy the return value. The memory of this value will be deallocated in the deallocateData function
		char * retVal = new char[response.length() + 1];
		strcpy_s(retVal, response.length() + 1, response.c_str());
		return retVal;
	};

	_declspec(dllexport) const char *getServiceURL(void)
	{
		return ServiceBase::GetInstance().getServiceURL();
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
	_declspec(dllexport) int init(const char * _ownIP, const char * _databaseIP, const char * _databasePWD, const char * _loggerServiceURL)
	{
		std::string loggerServiceURL(_loggerServiceURL);

#if defined(_DEBUG) || defined(OT_RELEASE_DEBUG)
	std::cout << "Authorization Service" << std::endl;
	ot::ServiceLogNotifier::initialize(OT_INFO_SERVICE_TYPE_AuthorisationService, loggerServiceURL, true);
#else
	ot::ServiceLogNotifier::initialize(OT_INFO_SERVICE_TYPE_AuthorisationService, loggerServiceURL, false);
#endif
		return ServiceBase::GetInstance().initialize(_ownIP, _databaseIP, _databasePWD);
	}
}
