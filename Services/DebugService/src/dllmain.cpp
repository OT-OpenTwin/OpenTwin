// @otlicense
// File: dllmain.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// DebugService header
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

	_declspec(dllexport) const char *performAction(const char * _json, const char * _senderIP) {
		Application::instance().actionAboutToBePerformed(_json);
		return ot::foundation::performAction(_json, _senderIP);
	};

	_declspec(dllexport) const char *queueAction(const char * _json, const char * _senderIP) {
		Application::instance().actionAboutToBePerformed(_json);
		return ot::foundation::performAction(_json, _senderIP);
	};

	_declspec(dllexport) const char *getServiceURL(void) {
		return ot::foundation::getServiceURL();
	}

	_declspec(dllexport) void deallocateData(const char * _data) {
		// *****************
		// This code will deallocate the memory of the return values from the perform- and queueAction calls
		if (_data != nullptr) {
			delete[] _data;
		}
		// *****************
	};

	// This function is called once upon startup of this service
	_declspec(dllexport) int init(const char* _ownUrl, const char* _unused1, const char* _unused2, const char* _unused3) {
		return ot::foundation::init(_ownUrl, &Application::instance(), false);
	}
}