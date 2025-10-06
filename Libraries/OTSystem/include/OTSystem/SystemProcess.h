//! \file Application.h
//! \author Alexander Kuester (alexk95)
//! \date April 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "OTSystem/SystemAPIExport.h"
#include "OTSystem/SystemTypes.h"
#include "OTSystem/RunResult.h"
#include <string>

#pragma warning(disable:4251)

namespace ot {

	//! \namespace app
	//! \brief The app namespace contains several functions that may be used to start processes.
	class OT_SYS_API_EXPORT SystemProcess
	{
	public:
	
	//! \brief Will start the application under the given application path.
	//! \param _applicationPath The path the application is located at.
	//! \param _waitForResponse If true, the function will wait until the process was successfully started.
	static RunResult runApplication(const std::string& _applicationPath);

	//! \brief Will start the application under the given application path.
	//! \param _applicationPath The path the application is located at.
	//! \param _waitForResponse If true, the function will wait until the process was successfully started.
	static RunResult runApplication(const std::wstring& _applicationPath);

	//! \brief Will start the application under the given application path with the given command line.
	//! \param _applicationPath The path the application is located at.
	//! \param _commandLine The command line that should be used to start the application.
	//! \param _hProcess The process handle will be written here.
	//! \param _waitForResponse If true, the function will wait until the process was successfully started.
	static RunResult runApplication(const std::string& _applicationPath, const std::string& _commandLine, OT_PROCESS_HANDLE& _processHandle);

	//! \brief Will start the application under the given application path with the given command line.
	//! \param _applicationPath The path the application is located at.
	//! \param _commandLine The command line that should be used to start the application.
	//! \param _hProcess The process handle will be written here.
	//! \param _waitForResponse If true, the function will wait until the process was successfully started.
	static RunResult runApplication(const std::wstring& _applicationPath, const std::wstring& _commandLine, OT_PROCESS_HANDLE& _processHandle);

	//! @brief Returns true if the provided process is still alive.
	static bool isApplicationRunning(OT_PROCESS_HANDLE& _processHandle);

	};
}