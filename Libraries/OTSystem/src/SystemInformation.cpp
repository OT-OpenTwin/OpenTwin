
#include "OTSystem/SystemInformation.h"
#include "OTSystem/Application.h"

#include <windows.h>
#include <psapi.h>
#include <thread>
#include <filesystem>
#include <fstream>

ot::SystemInformation::SystemInformation() :
	m_initialized(false),
	m_cpuQuery(0),
	m_cpuTotal(0),
	m_numProcessors(0),
	m_self(0),
	m_lastCPU({0}),
	m_lastSysCPU({0}),
	m_lastUserCPU({0})
{

}

ot::SystemInformation::~SystemInformation()
{
}

void ot::SystemInformation::initialize()
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

void ot::SystemInformation::getCurrentProcessCPUAndMemoryLoad(double& cpuLoad, double& memoryLoad)
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

void ot::SystemInformation::getGlobalCPUAndMemoryLoad(double& cpuLoad, double& memoryLoad)
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

std::string ot::SystemInformation::getBuildInformation()
{
	std::string buildInfo;

	std::string fileName = ot::app::getCurrentExecutableDirectory() + "\\BuildInfo.txt";

	// Chyeck whether local build information file exists
	if (!std::filesystem::exists(fileName))
	{
		// Get the development root environment variable and build the path to the deployment cert file
		char buffer[4096];
		size_t environmentVariableValueStringLength;

		getenv_s(&environmentVariableValueStringLength, buffer, sizeof(buffer) - 1, "OPENTWIN_DEV_ROOT");

		std::string dev_root(buffer);
		fileName = dev_root + "\\Deployment\\BuildInfo.txt";
	}

	try
	{
		std::ifstream file(fileName);
		if (!file.is_open()) throw std::exception();

		while (!file.eof())
		{
			std::string line;
			std::getline(file, line);

			buildInfo.append("\n");
			buildInfo.append(line);
		}
	}
	catch (std::exception)
	{
		// The build info file could not be found, return an empty string
	}

	return buildInfo;
}

