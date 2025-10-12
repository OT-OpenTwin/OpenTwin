
#pragma once

#include "OTSystem/SystemAPIExport.h"
#include "OTSystem/SystemTypes.h"

#include <pdh.h>

#include <string>

#undef GetObject

#pragma warning (disable:4251)

namespace ot {

	//! @class SystemLoadInformation
	//! @brief The SystemLoadInformation can be used to determine information about the system (e.g. cpu and memory load, build information).
	class OT_SYS_API_EXPORT SystemInformation {
	public:
		SystemInformation();
		virtual ~SystemInformation();

		//! @brief Initialize the performance counters. Needs to be called before information can be obtained
		void initialize();

		//! @brief Determine the current global system cpu and memory load in percent
		void getGlobalCPUAndMemoryLoad(double& cpuLoad, double& memoryLoad);

		//! @brief Determine the current process cpu and memory load in percent
		void getCurrentProcessCPUAndMemoryLoad(double& cpuLoad, double& memoryLoad);

		//! @brief Determine information about the currently used software build
		std::string getBuildInformation();

	private:
		bool m_initialized;
		PDH_HQUERY m_cpuQuery;
		PDH_HCOUNTER m_cpuTotal;
		ULARGE_INTEGER m_lastCPU, m_lastSysCPU, m_lastUserCPU;
		int m_numProcessors;
		HANDLE m_self;
	};
}
