//! @file OperatingSystem.h
//! @author Alexander Kuester (alexk95)
//! @date April 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTSystem/SystemAPIExport.h"

// std header
#include <string>

namespace ot {

	//! @class OperatingSystem
	//! @brief The os may be used to get different informations about the Hardware or System configuration.
	class OT_SYS_API_EXPORT OperatingSystem {
	public:

		//! @brief Returns the total physical memory of the current machine in bytes.
		static unsigned long long getTotalPhysicalMemory(void);

		//! @brief Returns the available physical memory of the current machine in bytes.
		static unsigned long long getAvailablePhysicalMemory(void);

		//! @brief Returns the total virtual memory of the current machine in bytes.
		static unsigned long long getTotalVirtualMemory(void);

		//! @brief Returns the available virtual memory of the current machine in bytes.
		static unsigned long long getAvailableVirtualMemory(void);

		//! @param _variableName Name of the variable.
		//! @return Null if variable is not found otherwise the caller takes ownership of the pointer.
		static char* getEnvironmentVariable(const char* _variableName);

		//! @param _variableName Name of the variable.
		static std::string getEnvironmentVariableString(const char* _variableName);

		//! @brief Will return a string in native format containing the current directory where the current executable is located at.
		static std::string getCurrentExecutableDirectory();

		//! @brief Will return a wide string in native format containing the current directory where the current executable is located at.
		static std::wstring getCurrentExecutableDirectoryW();

		//! @brief Will return a string in native format containing the path of the current executable.
		static std::string getCurrentExecutableFilePath();

		//! @brief Will return a wide string in native format containing the path of the current executable.
		static std::wstring getCurrentExecutableFilePathW();

		//! @brief Returns the current process ID.
		static unsigned long long getCurrentProcessID();
	};
}
