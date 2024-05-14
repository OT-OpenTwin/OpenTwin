//! \file OperatingSystem.h
//! \author Alexander Kuester (alexk95)
//! \date April 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "OTSystem/SystemAPIExport.h"
#include <string>

namespace ot {

	//! \namespace os
	//! \brief The os may be used to get different informations about the Hardware or System configuration.
	namespace os {

		//! \brief Returns the total physical memory of the current machine in bytes.
		OT_SYS_API_EXPORT unsigned long long getTotalPhysicalMemory(void);

		//! \brief Returns the available physical memory of the current machine in bytes.
		OT_SYS_API_EXPORT unsigned long long getAvailablePhysicalMemory(void);

		//! \brief Returns the total virtual memory of the current machine in bytes.
		OT_SYS_API_EXPORT unsigned long long getTotalVirtualMemory(void);

		//! \brief Returns the available virtual memory of the current machine in bytes.
		OT_SYS_API_EXPORT unsigned long long getAvailableVirtualMemory(void);

		//! \param _variableName
		//! \return Null if variable is not found
		OT_SYS_API_EXPORT char * getEnvironmentVariable(const char * _variableName);

		//! \brief Returns the path of the current executable.
		OT_SYS_API_EXPORT std::string getExecutablePath();
	}
}
