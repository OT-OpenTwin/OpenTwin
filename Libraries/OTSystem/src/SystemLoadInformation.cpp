
#include "OTSystem/SystemLoadInformation.h"

#include <windows.h>
#include <psapi.h>
#include <thread>

ot::SystemLoadInformation::SystemLoadInformation() :
	m_initialized(false),
	m_cpuQuery(0),
	m_cpuTotal(0),
	m_numProcessors(0),
	m_self(0)
{

}

ot::SystemLoadInformation::~SystemLoadInformation()
{
}

void ot::SystemLoadInformation::initialize()
{
	// Initialize information for current process data
	SYSTEM_INFO sysInfo;
	FILETIME ftime, fsys, fuser;

	GetSystemInfo(&sysInfo);
	m_numProcessors = sysInfo.dwNumberOfProcessors;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&m_lastCPU, &ftime, sizeof(FILETIME));

	m_self = GetCurrentProcess();
	GetProcessTimes(m_self, &ftime, &ftime, &fsys, &fuser);
	memcpy(&m_lastSysCPU, &fsys, sizeof(FILETIME));
	memcpy(&m_lastUserCPU, &fuser, sizeof(FILETIME));

	// Initialize global information
	PdhOpenQuery(NULL, NULL, &m_cpuQuery);
	// You can also use L"\\Processor(*)\\% Processor Time" and get individual CPU values with PdhGetFormattedCounterArray()
	PdhAddEnglishCounter(m_cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &m_cpuTotal);
	PdhCollectQueryData(m_cpuQuery);

	m_initialized = true;
}

void ot::SystemLoadInformation::getCurrentProcessCPUAndMemoryLoad(double& cpuLoad, double& memoryLoad)
{
	if (!m_initialized)
	{
		initialize();
	}

	// Determine the process memory load
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	DWORDLONG totalPhysMem = memInfo.ullTotalPhys;

	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	SIZE_T physMemUsedByMe = pmc.WorkingSetSize;

	memoryLoad = 100.0 * physMemUsedByMe / totalPhysMem;

	// Determine the process cpu load
	FILETIME ftime, fsys, fuser;
	ULARGE_INTEGER now, sys, user;
	double percent;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&now, &ftime, sizeof(FILETIME));

	GetProcessTimes(m_self, &ftime, &ftime, &fsys, &fuser);
	memcpy(&sys, &fsys, sizeof(FILETIME));
	memcpy(&user, &fuser, sizeof(FILETIME));
	percent = 1.0 * (sys.QuadPart - m_lastSysCPU.QuadPart) + (user.QuadPart - m_lastUserCPU.QuadPart);
	if (now.QuadPart != m_lastCPU.QuadPart)
	{
		percent /= (now.QuadPart - m_lastCPU.QuadPart);
	}
	else
	{
		percent = 0;
	}

	percent /= m_numProcessors;
	m_lastCPU = now;
	m_lastUserCPU = user;
	m_lastSysCPU = sys;

	cpuLoad = percent * 100;
}

void ot::SystemLoadInformation::getGlobalCPUAndMemoryLoad(double& cpuLoad, double& memoryLoad)
{
	if (!m_initialized)
	{
		initialize();
	}

	// Determine the global memory load
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);

	DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
	DWORDLONG physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;

	memoryLoad = 100.0 * physMemUsed / totalPhysMem;

	// Determine the global processor load
	PDH_FMT_COUNTERVALUE counterVal;

	PdhCollectQueryData(m_cpuQuery);
	PdhGetFormattedCounterValue(m_cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
	cpuLoad = counterVal.doubleValue;
}
