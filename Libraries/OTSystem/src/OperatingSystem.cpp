//! @file OperatingSystem.cpp
//! @author Alexander Kuester (alexk95)
//! @date April 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTSystem/OperatingSystem.h"
#include "OTSystem/ArchitectureInfo.h"

// std header
#include <codecvt>

#if defined(OT_OS_WINDOWS)
// Windows header
#include <Windows.h>
#include <TCHAR.h>
#include <pdh.h>

#elif defined(OT_OS_UNIX)
// Unix header header
#include <unistd.h>
#include <cstdlib>

#elif defined(OT_OS_LINUX)
// Linux header header
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <cstdlib>

#endif


unsigned long long ot::OperatingSystem::getTotalPhysicalMemory(void) {
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

unsigned long long ot::OperatingSystem::getAvailablePhysicalMemory(void) {
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

unsigned long long ot::OperatingSystem::getTotalVirtualMemory(void) {
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

unsigned long long ot::OperatingSystem::getAvailableVirtualMemory(void) {
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

char * ot::OperatingSystem::getEnvironmentVariable(const char * _variableName) {
#if defined(OT_OS_WINDOWS)
	char * ret = nullptr;
	size_t ct = 0;
	//_dupenv_s is return 0 on success
	if (_dupenv_s(&ret, &ct, _variableName) != 0) {
		if (ret) delete ret;
		return nullptr;
	}
	return ret;
#endif
}

std::string ot::OperatingSystem::getEnvironmentVariableString(const char* _variableName) {
	std::string result;
	char* var = OperatingSystem::getEnvironmentVariable(_variableName);
	if (var) {
		result.append(var);
		free(var);
	}
	return result;
}

std::string ot::OperatingSystem::getExecutablePath(void) {
#if defined(OT_OS_WINDOWS)
	TCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buffer, MAX_PATH);
	
	//Separate directory path from executable name
	std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
	std::wstring path = (std::wstring(buffer).substr(0, pos));
	
	//Only UTF8 character are supported by OpenTwin so far.
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	return myconv.to_bytes(path);
		
#else
	#error "Not supported yet"
#endif
}

