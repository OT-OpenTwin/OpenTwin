// @otlicense
// File: SystemInformation.cpp
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

// OpenTwin header
#include "OTSystem/SystemProcess.h"
#include "OTSystem/OperatingSystem.h"
#include "OTSystem/SystemInformation.h"

// std header
#include <thread>
#include <fstream>
#include <filesystem>

#if defined(OT_OS_WINDOWS)
#include <psapi.h>
#include <windows.h>
#elif defined(OT_OS_LINUX)
#include <unistd.h>
#include <sys/times.h>
#endif

ot::SystemInformation::SystemInformation() :
	m_initialized(false)
#if defined(OT_OS_WINDOWS)
	, m_cpuQuery(0),
	m_cpuTotal(0),
	m_numProcessors(0),
	m_self(0),
	m_lastCPU({0}),
	m_lastSysCPU({0}),
	m_lastUserCPU({0})
#elif defined(OT_OS_LINUX)
	, m_lastTotalTicks(0),
	m_lastIdleTicks(0),
	m_lastCPU(0),
	m_lastSysCPU(0),
	m_lastUserCPU(0),
	m_numProcessors(0)
#endif
{

}

ot::SystemInformation::~SystemInformation()
{
}

#if defined(OT_OS_WINDOWS)
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

#elif defined(OT_OS_LINUX)

namespace {
	bool readProcessorTicks(unsigned long long& _total, unsigned long long& _idle) {
		std::ifstream stat("/proc/stat");
		std::string cpu;
		unsigned long long user = 0, nice = 0, system = 0, idle = 0, iowait = 0, irq = 0, softirq = 0, steal = 0;
		if (!(stat >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal) || cpu != "cpu") {
			return false;
		}
		_idle = idle + iowait;
		_total = user + nice + system + idle + iowait + irq + softirq + steal;
		return true;
	}
}

void ot::SystemInformation::initialize()
{
	// Initialize information for current process data
	struct tms timeSample;
	m_lastCPU = static_cast<long>(times(&timeSample));
	m_lastSysCPU = static_cast<long>(timeSample.tms_stime);
	m_lastUserCPU = static_cast<long>(timeSample.tms_utime);
	const long processors = sysconf(_SC_NPROCESSORS_ONLN);
	m_numProcessors = processors > 0 ? static_cast<int>(processors) : 1;

	readProcessorTicks(m_lastTotalTicks, m_lastIdleTicks);

	m_initialized = true;
}

void ot::SystemInformation::getCurrentProcessCPUAndMemoryLoad(double& cpuLoad, double& memoryLoad)
{
	if (!m_initialized)
	{
		initialize();
	}

	const unsigned long long totalPhysMem = ot::OperatingSystem::getTotalPhysicalMemory();
	unsigned long long pages = 0, residentPages = 0;
	std::ifstream statm("/proc/self/statm");
	statm >> pages >> residentPages;
	const unsigned long long physMemUsedByMe = residentPages * static_cast<unsigned long long>(sysconf(_SC_PAGESIZE));

	memoryLoad = totalPhysMem > 0 ? 100.0 * physMemUsedByMe / totalPhysMem : 0.0;

	// Determine the process cpu load
	struct tms timeSample;
	const long now = static_cast<long>(times(&timeSample));
	const long sys = static_cast<long>(timeSample.tms_stime);
	const long user = static_cast<long>(timeSample.tms_utime);
	double percent = 1.0 * (sys - m_lastSysCPU) + (user - m_lastUserCPU);
	if (now != m_lastCPU)
	{
		percent /= (now - m_lastCPU);
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
	const unsigned long long totalPhysMem = ot::OperatingSystem::getTotalPhysicalMemory();
	const unsigned long long availablePhysMem = ot::OperatingSystem::getAvailablePhysicalMemory();
	const unsigned long long physMemUsed = totalPhysMem > availablePhysMem ? totalPhysMem - availablePhysMem : 0;

	memoryLoad = totalPhysMem > 0 ? 100.0 * physMemUsed / totalPhysMem : 0.0;

	unsigned long long total = 0, idle = 0;
	if (readProcessorTicks(total, idle) && total != m_lastTotalTicks)
	{
		cpuLoad = 100.0 * ((total - m_lastTotalTicks) - (idle - m_lastIdleTicks)) / (total - m_lastTotalTicks);
	}
	else
	{
		cpuLoad = 0.0;
	}
	m_lastTotalTicks = total;
	m_lastIdleTicks = idle;
}

#endif

std::string ot::SystemInformation::getBuildInformation()
{
	std::string buildInfo;

#if defined(OT_OS_WINDOWS)
	std::string fileName = ot::OperatingSystem::getCurrentExecutableDirectory() + "\\BuildInfo.txt";
#else
	std::string fileName = ot::OperatingSystem::getCurrentExecutableDirectory() + "/BuildInfo.txt";
#endif

	// Chyeck whether local build information file exists
	if (!std::filesystem::exists(fileName))
	{
		// Get the development root environment variable and build the path to the deployment cert file
		std::string dev_root = ot::OperatingSystem::getEnvironmentVariableString("OPENTWIN_DEV_ROOT");
#if defined(OT_OS_WINDOWS)
		fileName = dev_root + "\\Deployment\\BuildInfo.txt";
#else
		fileName = dev_root + "/Deployment/BuildInfo.txt";
#endif
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

