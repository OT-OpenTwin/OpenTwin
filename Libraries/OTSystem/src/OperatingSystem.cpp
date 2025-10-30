// @otlicense
// File: OperatingSystem.cpp
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
#include "OTSystem/Exception.h"
#include "OTSystem/OperatingSystem.h"
#include "OTSystem/ArchitectureInfo.h"

// std header
#include <codecvt>
#include <algorithm>

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

std::string ot::OperatingSystem::getCurrentExecutableDirectory() {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> strconverter;
	return strconverter.to_bytes(getCurrentExecutableDirectoryW());
}

std::wstring ot::OperatingSystem::getCurrentExecutableDirectoryW() {
	std::wstring currentDir = getCurrentExecutableFilePathW();

	// Trim the executable name
	size_t index = currentDir.find_last_of(L"\\/");
	if (index != std::wstring::npos) {
		currentDir = currentDir.substr(0, index);
	}

	return currentDir;
}

std::string ot::OperatingSystem::getCurrentExecutableFilePath() {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> strconverter;
	return strconverter.to_bytes(getCurrentExecutableFilePathW());
}

std::wstring ot::OperatingSystem::getCurrentExecutableFilePathW() {
	WCHAR currentExeFileName[MAX_PATH];
	GetModuleFileName(NULL, currentExeFileName, MAX_PATH);

	return std::wstring(currentExeFileName);
}

unsigned long long ot::OperatingSystem::getCurrentProcessID() {
	unsigned long long processID = 0;
	HANDLE handle = GetCurrentProcess();
	if (handle != nullptr) {
		processID = GetProcessId(handle);
		CloseHandle(handle);
	}
	return processID;
}
