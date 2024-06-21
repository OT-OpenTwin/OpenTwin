
#pragma once

#include "OTSystem/SystemAPIExport.h"
#include "OTSystem/SystemTypes.h"

#include <pdh.h>

#undef GetObject

#pragma warning (disable:4251)

namespace ot {

	//! \class SystemLoadInformation
	//! \brief The SystemLoadInformation can be used to determine information about the system load (cpu and memory).
	class OT_SYS_API_EXPORT SystemLoadInformation {
	public:
		SystemLoadInformation();
		virtual ~SystemLoadInformation();

		//! @brief Initialize the performance counters. Needs to be called before information can be obtained
		void initialize();

		//! @brief Determine the current global system cpu and memory load in percent
		void getGlobalCPUAndMemoryLoad(double& cpuLoad, double& memoryLoad);

		//! @brief Determine the current process cpu and memory load in percent
		void getCurrentProcessCPUAndMemoryLoad(double& cpuLoad, double& memoryLoad);

	private:
		bool m_initialized;
		PDH_HQUERY m_cpuQuery;
		PDH_HCOUNTER m_cpuTotal;
		ULARGE_INTEGER m_lastCPU, m_lastSysCPU, m_lastUserCPU;
		int m_numProcessors;
		HANDLE m_self;
	};
}
