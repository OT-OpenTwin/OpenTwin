#include "OpenTwinSystem/OperatingSystem.h"
#include "OpenTwinSystem/ArchitectureInfo.h"

#if defined(OT_OS_WINDOWS)
#include <Windows.h>
#include <TCHAR.h>
#include <pdh.h>
#elif defined(OT_OS_UNIX)
#include <unistd.h>
#include <cstdlib>
#elif defined(OT_OS_LINUX)
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <cstdlib>
#endif


unsigned long long ot::os::getTotalPhysicalMemory(void) {
#if defined(OT_OS_WINDOWS)
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);
	return status.ullTotalPhys;
#elif defined (OT_OS_UNIX)
	long pages = sysconf(_SC_PHYS_PAGES);
	long page_size = sysconf(_SC_PAGE_SIZE);
	return pages * page_size;
#elif defined (OT_OS_LINUX)
	struct sysinfo memInfo;
	sysinfo(&memInfo);
	long long totalPhysMem = memInfo.totalram;
	totalPhysMem *= memInfo.mem_unit;
	return totalPhysMem;
#endif
}

unsigned long long ot::os::getAvailablePhysicalMemory(void) {
#if defined(OT_OS_WINDOWS)
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);
	return status.ullAvailPhys;
#elif defined (OT_OS_UNIX)
	assert(0);
	return 0;
#elif defined (OT_OS_LINUX)
	struct sysinfo memInfo;
	sysinfo(&memInfo);
	long long availablePhysMem = memInfo.freeram;
	availablePhysMem *= memInfo.mem_unit;
	return availablePhysMem;
#endif
}

unsigned long long ot::os::getTotalVirtualMemory(void) {
#if defined(OT_OS_WINDOWS)
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);
	return status.ullTotalVirtual;
#elif defined (OT_OS_UNIX)
	assert(0);
	return 0;
#elif defined (OT_OS_LINUX)
	struct sysinfo memInfo;
	sysinfo(&memInfo);
	long long totalVirtualMem = memInfo.totalram;
	totalVirtualMem += memInfo.totalswap;
	totalVirtualMem *= memInfo.mem_unit;
	return totalVirtualMem;
#endif
}

unsigned long long ot::os::getAvailableVirtualMemory(void) {
#if defined(OT_OS_WINDOWS)
	MEMORYSTATUSEX status;
	status.dwLength = sizeof(status);
	GlobalMemoryStatusEx(&status);
	return status.ullAvailVirtual;
#elif defined (OT_OS_UNIX)
	assert(0);
	return 0;
#elif defined (OT_OS_LINUX)
	struct sysinfo memInfo;
	sysinfo(&memInfo);
	long long availableVirtualMem = memInfo.freeram;
	availableVirtualMem += memInfo.freeswap;
	availableVirtualMem *= memInfo.mem_unit;
	return availableVirtualMem;
#endif
}

char * ot::os::getEnvironmentVariable(const char * _variableName) {
#if defined(OT_OS_WINDOWS)
	char * ret = nullptr;
	size_t ct = 0;
	if (_dupenv_s(&ret, &ct, _variableName) != 0) {
		if (ret) delete ret;
		return nullptr;
	}
	return ret;
#endif
}

