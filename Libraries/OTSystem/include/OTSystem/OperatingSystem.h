#pragma once

#include "OTSystem/SystemAPIExport.h"
#include <string>

namespace ot {
	namespace os {

		OT_SYS_API_EXPORT unsigned long long getTotalPhysicalMemory(void);
		OT_SYS_API_EXPORT unsigned long long getAvailablePhysicalMemory(void);

		OT_SYS_API_EXPORT unsigned long long getTotalVirtualMemory(void);
		OT_SYS_API_EXPORT unsigned long long getAvailableVirtualMemory(void);

		/// <summary>
		/// 
		/// </summary>
		/// <param name="_variableName"></param>
		/// <returns>Null if variable is not found</returns>
		OT_SYS_API_EXPORT char * getEnvironmentVariable(const char * _variableName);
		OT_SYS_API_EXPORT std::string getExecutablePath();
	}
}
