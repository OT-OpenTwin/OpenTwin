//! \file SystemProcess.cpp
//! \author Alexander Kuester (alexk95)
//! \date April 2023
// ###########################################################################################################################################################################################################################################################################################################################

#include "OTSystem/SystemProcess.h"

#include <assert.h>
#include <string>
#include <iostream>
#include <string>
#include <fstream>
#include <cassert>
#include <locale>
#include <codecvt>
#include <sstream>

#include <UserEnv.h>

ot::SystemProcess::RunResult ot::SystemProcess::runApplication(const std::string& _applicationPath, bool _waitForResponse, unsigned long _waitTimeout) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> strconverter;
	return SystemProcess::runApplication(strconverter.from_bytes(_applicationPath), _waitForResponse, _waitTimeout);
}

ot::SystemProcess::RunResult ot::SystemProcess::runApplication(const std::wstring& _applicationPath, bool _waitForResponse, unsigned long _waitTimeout) {
	RunResult result = ot::SystemProcess::GeneralError;
#ifdef OT_OS_WINDOWS
	std::wstring commandLine(L"CMD.exe /c \"");
	commandLine.append(_applicationPath).append(L"\"");

	OT_PROCESS_HANDLE processHandle;
	result = runApplication(_applicationPath, commandLine, processHandle, _waitForResponse, _waitTimeout);
	CloseHandle(processHandle);
#else
	assert(0); // Not implemented yet
#endif // OS_WINDOWS

	return result;
}

ot::SystemProcess::RunResult ot::SystemProcess::runApplication(const std::string& _applicationPath, const std::string& _commandLine, OT_PROCESS_HANDLE& _processHandle, bool _waitForResponse, unsigned long _waitTimeout) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> strconverter;
	return SystemProcess::runApplication(strconverter.from_bytes(_applicationPath), strconverter.from_bytes(_commandLine), _processHandle, _waitForResponse, _waitTimeout);
}

#if defined(OT_OS_WINDOWS)
ot::SystemProcess::RunResult ot::SystemProcess::runApplication(const std::wstring& _applicationPath, const std::wstring& _commandLine, OT_PROCESS_HANDLE& _processHandle, bool _waitForResponse, unsigned long _waitTimeout) {
	HANDLE hToken = NULL;
	BOOL ok = OpenProcessToken(GetCurrentProcess(), TOKEN_READ, &hToken);
	assert(ok);

	wchar_t* penv = nullptr;

	ok = CreateEnvironmentBlock((void**)&penv, hToken, TRUE);
	if (ok == FALSE) {
		assert(0);
		return GeneralError;
	}
	
	LPWSTR cl = new WCHAR[_commandLine.size() + 1];
	wcscpy_s(cl, _commandLine.size() + 1, _commandLine.c_str());

	LPWSTR app = new WCHAR[_applicationPath.size() + 1];
	wcscpy_s(app, _applicationPath.size() + 1, _applicationPath.c_str());

	STARTUPINFO info;
	PROCESS_INFORMATION processInfo;

	ZeroMemory(&info, sizeof(info));
	info.cb = sizeof(info);
	info.lpTitle = cl;
	info.dwFlags = STARTF_USESHOWWINDOW;
	#ifdef OT_RELEASE_DEBUG
		info.wShowWindow = SW_SHOW;
	#else
		info.wShowWindow = SW_HIDE;
	#endif
	ZeroMemory(&processInfo, sizeof(processInfo));

	RunResult result = SystemProcess::GeneralError;

	if (CreateProcess(app, cl, NULL, NULL, TRUE, CREATE_UNICODE_ENVIRONMENT | CREATE_NEW_CONSOLE | ABOVE_NORMAL_PRIORITY_CLASS, penv, NULL, &info, &processInfo)) {
		if (_waitForResponse) {
			DWORD response = WaitForSingleObject(processInfo.hProcess, _waitTimeout);
			switch (response)
			{
			case WAIT_OBJECT_0: result = SystemProcess::OK; break;
			case WAIT_ABANDONED:
			case WAIT_TIMEOUT:
			case WAIT_FAILED: 
			default: result = SystemProcess::GeneralError; break;
			}
		}
		else result = SystemProcess::OK;
		_processHandle = processInfo.hProcess;
		CloseHandle(processInfo.hThread);
	}
	else
	{
		result = SystemProcess::OK;
	}

	delete[] cl;
	cl = nullptr;

	DestroyEnvironmentBlock(penv);

	return result;
}

bool ot::SystemProcess::isApplicationRunning(OT_PROCESS_HANDLE& _processHandle) {
	DWORD exitCode;
	if (GetExitCodeProcess(_processHandle, &exitCode)) {
		return (exitCode == STILL_ACTIVE);
	}
	else {
		return false;
	}
}

#endif

std::string ot::SystemProcess::getCurrentExecutableDirectory(void) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> strconverter;
	return strconverter.to_bytes(getCurrentExecutableDirectoryW());
}

std::wstring ot::SystemProcess::getCurrentExecutableDirectoryW(void) {
	std::wstring currentDir = getCurrentExecutableFilePathW();

	// Trim the executable name
	size_t index = currentDir.rfind(L'\\');
	if (index != std::wstring::npos) currentDir = currentDir.substr(0, index);

	return currentDir;
}

std::string ot::SystemProcess::getCurrentExecutableFilePath(void) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> strconverter;
	return strconverter.to_bytes(getCurrentExecutableFilePathW());
}

std::wstring ot::SystemProcess::getCurrentExecutableFilePathW(void) {
	WCHAR currentExeFileName[MAX_PATH];
	GetModuleFileName(NULL, currentExeFileName, MAX_PATH);

	return std::wstring(currentExeFileName);
}

std::string ot::SystemProcess::runCommandAndCaptureOutput(const char* _cmd) {
	std::string result;
	char buffer[1024];

	// Open a pipe to read the command output
	FILE* pipe = _popen(_cmd, "r");
	if (!pipe) {
		throw std::runtime_error("popen() failed!");
	}

	// Read output line by line
	while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
		result += buffer;
	}

	// Close the pipe
	_pclose(pipe);

	return result;
}
