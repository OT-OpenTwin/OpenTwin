//! \file Application.cpp
//! \author Alexander Kuester (alexk95)
//! \date April 2023
// ###########################################################################################################################################################################################################################################################################################################################

#include "OTSystem/Application.h"

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


ot::app::RunResult ot::app::runApplication(const std::string& _applicationPath) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> strconverter;
	return runApplication(strconverter.from_bytes(_applicationPath));
}

ot::app::RunResult ot::app::runApplication(const std::wstring& _applicationPath) {
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

ot::app::RunResult ot::app::runApplication(const std::string& _applicationPath, const std::string& _commandLine, OT_PROCESS_HANDLE& _processHandle) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> strconverter;
	return runApplication(strconverter.from_bytes(_applicationPath), strconverter.from_bytes(_commandLine), _processHandle);
}

#if defined(OT_OS_WINDOWS)
ot::app::RunResult ot::app::runApplication(const std::wstring& _applicationPath, const std::wstring& _commandLine, OT_PROCESS_HANDLE& _processHandle) {
	HANDLE hToken = NULL;
	BOOL success = OpenProcessToken(GetCurrentProcess(), TOKEN_READ, &hToken);
	assert(success);

	wchar_t* penv = nullptr;
	ot::app::RunResult resultMessage;

	success = CreateEnvironmentBlock((void**)&penv, hToken, TRUE);
	if (!success) {
		assert(0);
		resultMessage =	GetLastError();
		resultMessage.m_message = "creating an environment failed";
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
	info.wShowWindow = SW_HIDE;
	ZeroMemory(&processInfo, sizeof(processInfo));

	const bool createSuccess = CreateProcess(applicationPath, commandline, NULL, NULL, TRUE, CREATE_UNICODE_ENVIRONMENT | CREATE_NEW_CONSOLE | ABOVE_NORMAL_PRIORITY_CLASS, penv, NULL, &info, &processInfo);
	
	if (createSuccess) 
	{
		_processHandle = processInfo.hProcess;
		CloseHandle(processInfo.hThread);
	}
	else
	{
		resultMessage = GetLastError();
		resultMessage.m_message = "starting a process failed";
	}

	delete[] commandline;
	commandline = nullptr;

	DestroyEnvironmentBlock(penv);

	return resultMessage;
}

bool ot::app::isApplicationRunning(OT_PROCESS_HANDLE& _processHandle) {
	DWORD exitCode;
	if (GetExitCodeProcess(_processHandle, &exitCode)) {
		return (exitCode == STILL_ACTIVE);
	}
	else {
		return false;
	}
}

#endif

std::string ot::app::getCurrentExecutableDirectory(void) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> strconverter;
	return strconverter.to_bytes(getCurrentExecutableDirectoryW());
}

std::wstring ot::app::getCurrentExecutableDirectoryW(void) {
	std::wstring currentDir = getCurrentExecutableFilePathW();

	// Trim the executable name
	size_t index = currentDir.rfind(L'\\');
	if (index != std::wstring::npos) currentDir = currentDir.substr(0, index);

	return currentDir;
}

std::string ot::app::getCurrentExecutableFilePath(void) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> strconverter;
	return strconverter.to_bytes(getCurrentExecutableFilePathW());
}

std::wstring ot::app::getCurrentExecutableFilePathW(void) {
	WCHAR currentExeFileName[MAX_PATH];
	GetModuleFileName(NULL, currentExeFileName, MAX_PATH);

	return std::wstring(currentExeFileName);
}