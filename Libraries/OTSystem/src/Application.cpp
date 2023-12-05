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

ot::app::RunResult ot::app::runApplication(const std::string& _applicationPath, bool _waitForResponse, unsigned long _waitTimeout) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> strconverter;
	return runApplication(strconverter.from_bytes(_applicationPath), _waitForResponse, _waitTimeout);
}

ot::app::RunResult ot::app::runApplication(const std::wstring& _applicationPath, bool _waitForResponse, unsigned long _waitTimeout) {
	RunResult result = ot::app::GeneralError;
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

ot::app::RunResult ot::app::runApplication(const std::string& _applicationPath, const std::string& _commandLine, OT_PROCESS_HANDLE& _processHandle, bool _waitForResponse, unsigned long _waitTimeout) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> strconverter;
	return runApplication(strconverter.from_bytes(_applicationPath), strconverter.from_bytes(_commandLine), _processHandle, _waitForResponse, _waitTimeout);
}

#if defined(OT_OS_WINDOWS)
ot::app::RunResult ot::app::runApplication(const std::wstring& _applicationPath, const std::wstring& _commandLine, OT_PROCESS_HANDLE& _processHandle, bool _waitForResponse, unsigned long _waitTimeout) {
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
	ZeroMemory(&processInfo, sizeof(processInfo));

	RunResult result = app::GeneralError;

	if (CreateProcess(app, cl, NULL, NULL, TRUE, CREATE_UNICODE_ENVIRONMENT | CREATE_NEW_CONSOLE, penv, NULL, &info, &processInfo))
	//if (CreateProcess(app, cl, NULL, NULL, TRUE, CREATE_UNICODE_ENVIRONMENT | CREATE_NO_WINDOW, penv, NULL, &info, &processInfo))
	{
		if (_waitForResponse) {
			DWORD response = WaitForSingleObject(processInfo.hProcess, _waitTimeout);
			switch (response)
			{
			case WAIT_OBJECT_0: result = app::OK; break;
			case WAIT_ABANDONED:
			case WAIT_TIMEOUT:
			case WAIT_FAILED: 
			default: result = app::GeneralError; break;
			}
		}
		else result = app::OK;
		_processHandle = processInfo.hProcess;
		CloseHandle(processInfo.hThread);
	}
	else
	{
		result = app::OK;
	}

	delete[] cl;
	cl = nullptr;

	DestroyEnvironmentBlock(penv);

	return result;
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