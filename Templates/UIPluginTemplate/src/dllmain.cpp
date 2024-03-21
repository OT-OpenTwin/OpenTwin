#include <Windows.h>

#include <openTwin/AbstractUIWidgetInterface.h>

#include "PluginCore.h"

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
	__declspec(dllexport) bool attachToUiService(ot::AbstractUIWidgetInterface * _interface) {
		PluginCore::instance()->setUiInterface(_interface);
		return PluginCore::instance()->initialize();
	}

	__declspec(dllexport) void detachFromUiService(void) {
		PluginCore::deleteInstance();
	}

	__declspec(dllexport) bool messageReceived(const char * _action, const char * _msg) {
		return PluginCore::instance()->dispatchAction(_action, _msg);
	}
}

