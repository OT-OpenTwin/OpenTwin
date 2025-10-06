//! \file Application.cpp
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


ot::RunResult ot::SystemProcess::runApplication(const std::string& _applicationPath) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> strconverter;
	return runApplication(strconverter.from_bytes(_applicationPath));
}

ot::RunResult ot::SystemProcess::runApplication(const std::wstring& _applicationPath) {
	RunResult result;
#ifdef OT_OS_WINDOWS
	std::wstring commandLine(L"CMD.exe /c \"");
	commandLine.append(_applicationPath).append(L"\"");

	OT_PROCESS_HANDLE processHandle;
	result = runApplication(_applicationPath, commandLine, processHandle);
	CloseHandle(processHandle);
#else
	assert(0); // Not implemented yet
#endif // OS_WINDOWS

	return result;
}

ot::RunResult ot::SystemProcess::runApplication(const std::string& _applicationPath, const std::string& _commandLine, OT_PROCESS_HANDLE& _processHandle) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> strconverter;
	return runApplication(strconverter.from_bytes(_applicationPath), strconverter.from_bytes(_commandLine), _processHandle);
}

#if defined(OT_OS_WINDOWS)
ot::RunResult ot::SystemProcess::runApplication(const std::wstring& _applicationPath, const std::wstring& _commandLine, OT_PROCESS_HANDLE& _processHandle) {
	HANDLE hToken = NULL;
	BOOL success = OpenProcessToken(GetCurrentProcess(), TOKEN_READ, &hToken);
	assert(success);

	wchar_t* penv = nullptr;
	ot::RunResult resultMessage;

	success = CreateEnvironmentBlock((void**)&penv, hToken, TRUE);
	if (!success) {
		assert(0);
		resultMessage.setAsError(GetLastError());
		resultMessage.setErrorMessage( "creating an environment failed");
		return resultMessage;
	}
	
	LPWSTR commandline = new WCHAR[_commandLine.size() + 1];
	wcscpy_s(commandline, _commandLine.size() + 1, _commandLine.c_str());

	LPWSTR applicationPath = new WCHAR[_applicationPath.size() + 1];
	wcscpy_s(applicationPath, _applicationPath.size() + 1, _applicationPath.c_str());

	STARTUPINFO info;
	PROCESS_INFORMATION processInfo;

	ZeroMemory(&info, sizeof(info));
	info.cb = sizeof(info);
	info.lpTitle = commandline;
	info.dwFlags = STARTF_USESHOWWINDOW;
	#ifdef OT_RELEASE_DEBUG
		info.wShowWindow = SW_SHOW;
	#else
		info.wShowWindow = SW_HIDE;
	#endif
	ZeroMemory(&processInfo, sizeof(processInfo));

	const bool createSuccess = CreateProcess(applicationPath, commandline, NULL, NULL, TRUE, CREATE_UNICODE_ENVIRONMENT | CREATE_NEW_CONSOLE | ABOVE_NORMAL_PRIORITY_CLASS, penv, NULL, &info, &processInfo);
	
	if (createSuccess) 
	{
		_processHandle = processInfo.hProcess;
		CloseHandle(processInfo.hThread);
	}
	else
	{
		resultMessage.setAsError(GetLastError());
		resultMessage.setErrorMessage("starting a process failed");
	}

	delete[] commandline;
	commandline = nullptr;

	DestroyEnvironmentBlock(penv);

	return resultMessage;
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
