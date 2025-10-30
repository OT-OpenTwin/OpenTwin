// @otlicense
// File: SystemInformation.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
