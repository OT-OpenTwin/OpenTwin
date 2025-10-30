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

// GDS header
#include "Application.h"

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTCommunication/ActionDispatcher.h"
#include "OTCommunication/ServiceLogNotifier.h"
#include "OTServiceFoundation/Foundation.h"

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

	_declspec(dllexport) const char *performAction(const char * _json, const char * _senderURL) {
		return ot::ActionDispatcher::instance().dispatchWrapper(_json, _senderURL, ot::EXECUTE);
	};

	_declspec(dllexport) const char *queueAction(const char * _json, const char * _senderURL)
	{
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
		// Deallocate the memory of the return values from the perform- and queueAction calls
		if (_data != nullptr)
		{
			delete[] _data;
		}
	};

	// This function is called once upon startup of this service
	_declspec(dllexport) int init(const char * _loggerServiceURL, const char * _ownURL, const char * _globalSessionServiceURL, const char * _unused2)
	{
		std::string loggerServiceURL(_loggerServiceURL);

#if defined(_DEBUG) || defined(OT_RELEASE_DEBUG)
		std::cout << "Global Directory Service" << std::endl;
		ot::ServiceLogNotifier::initialize(OT_INFO_SERVICE_TYPE_GlobalDirectoryService, loggerServiceURL, true);
#else
		ot::ServiceLogNotifier::initialize(OT_INFO_SERVICE_TYPE_GlobalDirectoryService, loggerServiceURL, false);
#endif
		return Application::instance().initialize("1", _ownURL, _globalSessionServiceURL);
	};
}