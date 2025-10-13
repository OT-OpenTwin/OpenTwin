//! @file dllmain.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

// Service header
#include "Application.h"

// OpenTwin header
#include "OTServiceFoundation/Foundation.h"

// std header
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

	_declspec(dllexport) const char *performAction(const char * _json, const char * _senderUrl) {
		return ot::foundation::performAction(_json, _senderUrl);
	};

	_declspec(dllexport) const char *queueAction(const char * _json, const char * _senderUrl) {
		return ot::foundation::performAction(_json, _senderUrl);
	};

	_declspec(dllexport) const char* getServiceURL() {
		return ot::foundation::getServiceURL();
	};

	_declspec(dllexport) void deallocateData(const char * _data) {
		if (_data != nullptr) {
			delete[] _data;
		}
	};

	_declspec(dllexport) int init(const char* _ownUrl, const char* _unused1, const char* _unused2, const char* _unused3) {
		return ot::foundation::init(_ownUrl, &Application::instance(), false);
	};
}