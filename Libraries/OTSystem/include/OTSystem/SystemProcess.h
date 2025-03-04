//! \file SystemProcess.h
//! \author Alexander Kuester (alexk95)
//! \date April 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "OTSystem/SystemAPIExport.h"
#include "OTSystem/SystemTypes.h"

#include <string>

namespace ot {

	//! \namespace app
	//! \brief The app namespace contains several functions that may be used to start processes.
	class OT_SYS_API_EXPORT SystemProcess {
	public:

		//! Describes the result of a run application request.
		enum RunResult {
			OK,				// No error occured, start successful.
			GeneralError	// Start failed.
		};

		//! \brief Will start the application under the given application path.
		//! \param _applicationPath The path the application is located at.
		//! \param _waitForResponse If true, the function will wait until the process was successfully started.
		static RunResult runApplication(const std::string& _applicationPath, bool _waitForResponse = true, unsigned long _waitTimeout = OT_INFINITE_TIMEOUT);

		//! \brief Will start the application under the given application path.
		//! \param _applicationPath The path the application is located at.
		//! \param _waitForResponse If true, the function will wait until the process was successfully started.
		static RunResult runApplication(const std::wstring& _applicationPath, bool _waitForResponse = true, unsigned long _waitTimeout = OT_INFINITE_TIMEOUT);

		//! \brief Will start the application under the given application path with the given command line.
		//! \param _applicationPath The path the application is located at.
		//! \param _commandLine The command line that should be used to start the application.
		//! \param _hProcess The process handle will be written here.
		//! \param _waitForResponse If true, the function will wait until the process was successfully started.
		static RunResult runApplication(const std::string& _applicationPath, const std::string& _commandLine, OT_PROCESS_HANDLE& _processHandle, bool _waitForResponse = true, unsigned long _waitTimeout = OT_INFINITE_TIMEOUT);

		//! \brief Will start the application under the given application path with the given command line.
		//! \param _applicationPath The path the application is located at.
		//! \param _commandLine The command line that should be used to start the application.
		//! \param _hProcess The process handle will be written here.
		//! \param _waitForResponse If true, the function will wait until the process was successfully started.
		static RunResult runApplication(const std::wstring& _applicationPath, const std::wstring& _commandLine, OT_PROCESS_HANDLE& _processHandle, bool _waitForResponse = true, unsigned long _waitTimeout = OT_INFINITE_TIMEOUT);

		//! @brief Returns true if the provided process is still alive.
		static bool isApplicationRunning(OT_PROCESS_HANDLE& _processHandle);

		//! \brief Will return a string containing the current directory where the current executable is located at.
		static std::string getCurrentExecutableDirectory(void);

		//! \brief Will return a wide string containing the current directory where the current executable is located at.
		static std::wstring getCurrentExecutableDirectoryW(void);

		//! \brief Will return a string containing the path of the current executable.
		static std::string getCurrentExecutableFilePath(void);

		//! \brief Will return a wide string containing the path of the current executable.
		static std::wstring getCurrentExecutableFilePathW(void);

		//! @brief Run the specified command and capute the generated output in a string.
		//! @param _cmd Command to execute.
		//! @return Captured output.
		static std::string runCommandAndCaptureOutput(const char* _cmd);

	};
}