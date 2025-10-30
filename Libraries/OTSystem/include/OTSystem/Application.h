// @otlicense
// File: Application.h
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

#include "OTSystem/SystemAPIExport.h"
#include "OTSystem/SystemTypes.h"
#include "OTSystem/RunResult.h"

#include <string>

#pragma warning(disable:4251)

namespace ot {

	//! \namespace app
	//! \brief The app namespace contains several functions that may be used to start processes.
	namespace app {

		//! \brief Will start the application under the given application path.
		//! \param _applicationPath The path the application is located at.
		//! \param _waitForResponse If true, the function will wait until the process was successfully started.
		OT_SYS_API_EXPORT RunResult runApplication(const std::string& _applicationPath);

		//! \brief Will start the application under the given application path.
		//! \param _applicationPath The path the application is located at.
		//! \param _waitForResponse If true, the function will wait until the process was successfully started.
		OT_SYS_API_EXPORT RunResult runApplication(const std::wstring& _applicationPath);

		//! \brief Will start the application under the given application path with the given command line.
		//! \param _applicationPath The path the application is located at.
		//! \param _commandLine The command line that should be used to start the application.
		//! \param _hProcess The process handle will be written here.
		//! \param _waitForResponse If true, the function will wait until the process was successfully started.
		OT_SYS_API_EXPORT RunResult runApplication(const std::string& _applicationPath, const std::string& _commandLine, OT_PROCESS_HANDLE& _processHandle);

		//! \brief Will start the application under the given application path with the given command line.
		//! \param _applicationPath The path the application is located at.
		//! \param _commandLine The command line that should be used to start the application.
		//! \param _hProcess The process handle will be written here.
		//! \param _waitForResponse If true, the function will wait until the process was successfully started.
		OT_SYS_API_EXPORT RunResult runApplication(const std::wstring& _applicationPath, const std::wstring& _commandLine, OT_PROCESS_HANDLE& _processHandle);

		//! @brief Returns true if the provided process is still alive.
		OT_SYS_API_EXPORT bool isApplicationRunning(OT_PROCESS_HANDLE& _processHandle);

		//! \brief Will return a string containing the current directory where the current executable is located at.
		OT_SYS_API_EXPORT std::string getCurrentExecutableDirectory(void);

		//! \brief Will return a wide string containing the current directory where the current executable is located at.
		OT_SYS_API_EXPORT std::wstring getCurrentExecutableDirectoryW(void);

		//! \brief Will return a string containing the path of the current executable.
		OT_SYS_API_EXPORT std::string getCurrentExecutableFilePath(void);

		//! \brief Will return a wide string containing the path of the current executable.
		OT_SYS_API_EXPORT std::wstring getCurrentExecutableFilePathW(void);

	}
}