//! \file Application.h
//! \author Alexander Kuester (alexk95)
//! \date April 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "OTSystem/SystemAPIExport.h"
#include "OTSystem/SystemTypes.h"

#include <string>

#pragma warning(disable:4251)

namespace ot {

	//! \namespace app
	//! \brief The app namespace contains several functions that may be used to start processes.
	class OT_SYS_API_EXPORT SystemProcess
	{
	public:
		//! Describes the result of a run application request.
		struct RunResult {
			uint32_t m_value = 0;
			std::string m_message = "";

			bool isOk()
			{
				return m_value == 0;
			}

			RunResult() = default;
			RunResult& operator=(const RunResult& _other) = default;
			RunResult& operator=(RunResult&& _other) = default;


			RunResult& operator=(const uint32_t& _value)
			{
				m_value = _value;
				return *this;
			}

			RunResult& operator=(uint32_t&& _value)
			{
				m_value = _value;
				return *this;
			}

			RunResult(const RunResult& _other) = default;
			RunResult(RunResult&& _other) = default;

			RunResult(const uint32_t& _other)
				:m_value(_other)
			{}

			bool operator==(const RunResult& _other)
			{
				return m_value == _other.m_value;
			}
			bool operator!=(const RunResult& _other)
			{
				return !(*this == _other);
			}
			};

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

	//! \brief Will return a string containing the current directory where the current executable is located at.
	static std::string getCurrentExecutableDirectory(void);

	//! \brief Will return a wide string containing the current directory where the current executable is located at.
	static std::wstring getCurrentExecutableDirectoryW(void);

	//! \brief Will return a string containing the path of the current executable.
	static std::string getCurrentExecutableFilePath(void);

	//! \brief Will return a wide string containing the path of the current executable.
	static std::wstring getCurrentExecutableFilePathW(void);

	};
}