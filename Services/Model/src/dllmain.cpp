// @otlicense

// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h" 

// Model header
#include "Application.h"

// OpenTwin header
#include "OTSystem/ArchitectureInfo.h"
#include "OTServiceFoundation/Foundation.h"

#ifdef OT_OS_WINDOWS
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
#endif // OT_OS_WINDOWS

extern "C" {

	_declspec(dllexport) const char* performAction(const char* _json, const char* _senderIP)
	{
		return ot::foundation::performAction(_json, _senderIP);
	};

	_declspec(dllexport) const char* queueAction(const char* _json, const char* _senderIP)
	{
		return ot::foundation::performAction(_json, _senderIP);
	};

	_declspec(dllexport) const char* getServiceURL(void)
	{
		return ot::foundation::getServiceURL();
	}

	_declspec(dllexport) void deallocateData(const char* _data)
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
	_declspec(dllexport) int init(const char* _ownUrl, const char* _unused1, const char* _unused2, const char* _unused3) {
		return ot::foundation::init(_ownUrl, Application::instance(), false);
	}
}