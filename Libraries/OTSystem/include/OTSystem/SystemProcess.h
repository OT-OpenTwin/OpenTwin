//! @file SystemProcess.h
//! @author Alexander Kuester (alexk95)
//! @date April 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTSystem/Flags.h"
#include "OTSystem/RunResult.h"
#include "OTSystem/SystemTypes.h"
#include "OTSystem/SystemAPIExport.h"

// std header
#include <string>

#pragma warning(disable:4251)

#if defined(OT_OS_WINDOWS)

#include <Windows.h>

//! @def OT_PROCESS_HANDLE
//! @brief Process handle
#define OT_PROCESS_HANDLE HANDLE

//! @def OT_INVALID_PROCESS_HANDLE
//! @brief Invalid process handle
#define OT_INVALID_PROCESS_HANDLE nullptr

//! @def OT_INFINITE_TIMEOUT
//! @brief Infinite timeout
#define OT_INFINITE_TIMEOUT INFINITE

namespace  ot {

	//! @typedef ExternalLibraryInstance_t
	//! @brief Instance type for a loaded external library.
	typedef HINSTANCE ExternalLibraryInstance_t;
}


#endif

namespace ot {

	//! @namespace app
	//! @brief The app namespace contains several functions that may be used to start processes.
	class OT_SYS_API_EXPORT SystemProcess {
	public:
		enum ProcessFlag {
			NoProcessFlags      = 0 << 0, //! @brief No flags are set.
			UseUnicode          = 1 << 0, //! @brief Use unicode for command line and application path.
			CreateNewConsole    = 1 << 1, //! @brief Create a new console for the application.
			DetachedProcess     = 1 << 2, //! @brief Detach the process from the parent process.
			AboveNormalPriority = 1 << 3, //! @brief Start the process with above normal priority.
			HideWindow          = 1 << 4, //! @brief Start the process with hidden window (if supported by application).

			//! @brief Default flags.
			DefaultFlags = UseUnicode | CreateNewConsole | AboveNormalPriority | HideWindow
		};
		typedef ot::Flags<ProcessFlag> ProcessFlags;

		//! @brief Will start the application under the given application path.
		//! @param _applicationPath The path the application is located at.
		//! @param _waitForResponse If true, the function will wait until the process was successfully started.
		static RunResult runApplication(const std::string& _applicationPath, const ProcessFlags& _flags = ProcessFlag::DefaultFlags);

		//! @brief Will start the application under the given application path.
		//! @param _applicationPath The path the application is located at.
		//! @param _waitForResponse If true, the function will wait until the process was successfully started.
		static RunResult runApplication(const std::wstring& _applicationPath, const ProcessFlags& _flags = ProcessFlag::DefaultFlags);

		//! @brief Will start the application under the given application path with the given command line.
		//! @param _applicationPath The path the application is located at.
		//! @param _commandLine The command line that should be used to start the application.
		//! @param _hProcess The process handle will be written here.
		//! @param _waitForResponse If true, the function will wait until the process was successfully started.
		static RunResult runApplication(const std::string& _applicationPath, const std::string& _commandLine, OT_PROCESS_HANDLE& _processHandle, const ProcessFlags& _flags = ProcessFlag::DefaultFlags);

		//! @brief Will start the application under the given application path with the given command line.
		//! @param _applicationPath The path the application is located at.
		//! @param _commandLine The command line that should be used to start the application.
		//! @param _hProcess The process handle will be written here.
		//! @param _waitForResponse If true, the function will wait until the process was successfully started.
		static RunResult runApplication(const std::wstring& _applicationPath, const std::wstring& _commandLine, OT_PROCESS_HANDLE& _processHandle, const ProcessFlags& _flags = ProcessFlag::DefaultFlags);

		//! @brief Returns true if the provided process is still alive.
		static bool isApplicationRunning(OT_PROCESS_HANDLE& _processHandle);

	};
}

OT_ADD_FLAG_FUNCTIONS(ot::SystemProcess::ProcessFlag)