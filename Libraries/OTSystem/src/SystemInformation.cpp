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
#include "OTSystem/Internal/SystemInformationPrivate.h"

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

ot::SystemInformation::SystemInformation()
	: m_data(new SystemInformationPrivate()) {}

ot::SystemInformation::SystemInformation(const SystemInformation& _other)
	: m_data(new SystemInformationPrivate(*_other.m_data))
{}

ot::SystemInformation::SystemInformation(SystemInformation && _other) noexcept
	: m_data(std::move(_other.m_data))
{
	_other.m_data.reset();;
}

ot::SystemInformation::~SystemInformation()
{}

ot::SystemInformation& ot::SystemInformation::operator=(const SystemInformation& _other)
{
	if (this != &_other)
	{
		m_data.reset(new SystemInformationPrivate(*_other.m_data));
	}
	return *this;
}

ot::SystemInformation& ot::SystemInformation::operator=(SystemInformation&& _other) noexcept
{
	if (this != &_other)
	{
		m_data = std::move(_other.m_data);
		_other.m_data.reset();
	}
	return *this;
}

#if defined(OT_OS_WINDOWS)
void ot::SystemInformation::initialize()
{
	OTAssertNullptr(m_data);

	// Initialize information for current process data
	SYSTEM_INFO sysInfo;
	FILETIME ftime, fsys, fuser;

	GetSystemInfo(&sysInfo);
	m_data->numProcessors = sysInfo.dwNumberOfProcessors;

	GetSystemTimeAsFileTime(&ftime);
	memcpy(&m_data->lastCPU, &ftime, sizeof(FILETIME));

	m_data->self = GetCurrentProcess();
	GetProcessTimes(m_data->self, &ftime, &ftime, &fsys, &fuser);
	memcpy(&m_data->lastSysCPU, &fsys, sizeof(FILETIME));
	memcpy(&m_data->lastUserCPU, &fuser, sizeof(FILETIME));

	// Initialize global information
	PdhOpenQuery(NULL, NULL, &m_data->cpuQuery);
	// You can also use L"\\Processor(*)\\% Processor Time" and get individual CPU values with PdhGetFormattedCounterArray()
	PdhAddEnglishCounter(m_data->cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &m_data->cpuTotal);
	PdhCollectQueryData(m_data->cpuQuery);

	m_data->initialized = true;
}

void ot::SystemInformation::getCurrentProcessCPUAndMemoryLoad(double& cpuLoad, double& memoryLoad)
{
	OTAssertNullptr(m_data);
	if (!m_data->initialized)
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

	GetProcessTimes(m_data->self, &ftime, &ftime, &fsys, &fuser);
	memcpy(&sys, &fsys, sizeof(FILETIME));
	memcpy(&user, &fuser, sizeof(FILETIME));
	percent = 1.0 * (sys.QuadPart - m_data->lastSysCPU.QuadPart) + (user.QuadPart - m_data->lastUserCPU.QuadPart);
	if (now.QuadPart != m_data->lastCPU.QuadPart)
	{
		percent /= (now.QuadPart - m_data->lastCPU.QuadPart);
	}
	else
	{
		percent = 0;
	}

	percent /= m_data->numProcessors;
	m_data->lastCPU = now;
	m_data->lastUserCPU = user;
	m_data->lastSysCPU = sys;

	cpuLoad = percent * 100;
}

void ot::SystemInformation::getGlobalCPUAndMemoryLoad(double& cpuLoad, double& memoryLoad)
{
	if (!m_data->initialized)
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

	PdhCollectQueryData(m_data->cpuQuery);
	PdhGetFormattedCounterValue(m_data->cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
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
	m_data->lastCPU = static_cast<long>(times(&timeSample));
	m_data->lastSysCPU = static_cast<long>(timeSample.tms_stime);
	m_data->lastUserCPU = static_cast<long>(timeSample.tms_utime);
	const long processors = sysconf(_SC_NPROCESSORS_ONLN);
	m_data->numProcessors = processors > 0 ? static_cast<int>(processors) : 1;

	readProcessorTicks(m_data->lastTotalTicks, m_data->lastIdleTicks);

	m_data->initialized = true;
}

void ot::SystemInformation::getCurrentProcessCPUAndMemoryLoad(double& cpuLoad, double& memoryLoad)
{
	OTAssertNullptr(m_data);
	if (!m_data->initialized)
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
	double percent = 1.0 * (sys - m_data->lastSysCPU) + (user - m_data->lastUserCPU);
	if (now != m_data->lastCPU)
	{
		percent /= (now - m_data->lastCPU);
	}
	else
	{
		percent = 0;
	}

	percent /= m_data->numProcessors;
	m_data->lastCPU = now;
	m_data->lastUserCPU = user;
	m_data->lastSysCPU = sys;

	cpuLoad = percent * 100;
}

void ot::SystemInformation::getGlobalCPUAndMemoryLoad(double& cpuLoad, double& memoryLoad)
{
	OTAssertNullptr(m_data);
	if (!m_data->initialized)
	{
		initialize();
	}

	// Determine the global memory load
	const unsigned long long totalPhysMem = ot::OperatingSystem::getTotalPhysicalMemory();
	const unsigned long long availablePhysMem = ot::OperatingSystem::getAvailablePhysicalMemory();
	const unsigned long long physMemUsed = totalPhysMem > availablePhysMem ? totalPhysMem - availablePhysMem : 0;

	memoryLoad = totalPhysMem > 0 ? 100.0 * physMemUsed / totalPhysMem : 0.0;

	unsigned long long total = 0, idle = 0;
	if (readProcessorTicks(total, idle) && total != m_data->lastTotalTicks)
	{
		cpuLoad = 100.0 * ((total - m_data->lastTotalTicks) - (idle - m_data->lastIdleTicks)) / (total - m_data->lastTotalTicks);
	}
	else
	{
		cpuLoad = 0.0;
	}
	m_data->lastTotalTicks = total;
	m_data->lastIdleTicks = idle;
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
	catch (const std::exception&)
	{
		// The build info file could not be found, return an empty string
	}

	return buildInfo;
}

