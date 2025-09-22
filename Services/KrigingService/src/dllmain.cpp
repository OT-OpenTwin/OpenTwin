/*
 * dllmain.cpp
 */

// C++ header
#include <Windows.h>

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTServiceFoundation/Foundation.h"

// Service header
#include "Application.h"

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
		OT_LOG_D("Called with: " + std::string(_json));
		return ot::foundation::performAction(_json, _senderIP);
	};

	_declspec(dllexport) const char *queueAction(const char * _json, const char * _senderIP)
	{
		OT_LOG_D("Called... forwarding to performAction");
		//ToDo: If you want to actually queue the command implement the queueing logic here
		// At this point we are simply forwarding the message to the perform action function to execute the action immediatly
		return ot::foundation::performAction(_json, _senderIP);
	};

	_declspec(dllexport) const char *getServiceURL(void)
	{
		return ot::foundation::getServiceURL();
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
	_declspec(dllexport) int init(const char * _localDirectoryServiceURL, const char * _ownIP, const char * _sessionServiceIP, const char * _sessionID)
	{
		return ot::foundation::init(_localDirectoryServiceURL, _ownIP, _sessionServiceIP, _sessionID, new Application(), false);
	};
}
