// @otlicense

#pragma once

// OpenTwin header
#include "OTSystem/ArchitectureInfo.h"

#if defined(OT_OS_WINDOWS)
#include <pdh.h>
#endif

namespace ot
{
	class SystemInformation;

	class SystemInformationPrivate
	{
	public:
		~SystemInformationPrivate() = default;
	private:
		SystemInformationPrivate() = default;
		SystemInformationPrivate(const SystemInformationPrivate&) = default;
		SystemInformationPrivate(SystemInformationPrivate&&) noexcept = default;
		SystemInformationPrivate& operator=(const SystemInformationPrivate&) = default;
		SystemInformationPrivate& operator=(SystemInformationPrivate&&) noexcept = default;

		friend class SystemInformation;

		bool initialized = false;

#if defined(OT_OS_WINDOWS)
		PDH_HQUERY cpuQuery = 0;
		PDH_HCOUNTER cpuTotal = 0;
		ULARGE_INTEGER lastCPU = { 0 };
		ULARGE_INTEGER lastSysCPU = { 0 };
		ULARGE_INTEGER lastUserCPU = { 0 };
		int numProcessors = 0;
		HANDLE self = nullptr;

#elif defined(OT_OS_LINUX)
		unsigned long long lastTotalTicks = 0;
		unsigned long long lastIdleTicks = 0;
		long lastCPU = 0;
		long lastSysCPU = 0;
		long lastUserCPU = 0;
		int numProcessors = 0;
#endif
	};

}