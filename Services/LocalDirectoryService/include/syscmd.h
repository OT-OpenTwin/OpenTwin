// @otlicense
// File: syscmd.h
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

#pragma once

// Required headers
#include <string>

// OS dependent header
#if defined _WIN32 || defined _WIN64

#define OS_IS_WINDOWS 1
#include <Windows.h>
#elif defined unix || defined __unix || defined __unix__



#endif
namespace sys {
	namespace cmd {
		bool runApp(
			const std::string				_filePath,
			bool							_waitForResponse = true
		);

		bool runApp(
			const std::string				_applicationPath,
			const std::string				_commandLine,
			HANDLE						   &_hProcess
		);

		std::string getCurrentExecutableDirectory(void);
	}
}
