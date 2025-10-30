// @otlicense

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