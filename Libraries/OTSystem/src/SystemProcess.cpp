// @otlicense
// File: SystemProcess.cpp
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

#if defined(OT_OS_WINDOWS)
#include <UserEnv.h>
#elif defined(OT_OS_LINUX)
#include <spawn.h>
#include <unistd.h>
#include <sys/wait.h>
#include <vector>
#endif


ot::RunResult ot::SystemProcess::runApplication(const std::string& _applicationPath, const ProcessFlags& _flags) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> strconverter;
	return runApplication(strconverter.from_bytes(_applicationPath), _flags);
}

ot::RunResult ot::SystemProcess::runApplication(const std::wstring& _applicationPath, const ProcessFlags& _flags) {
	RunResult result;
#ifdef OT_OS_WINDOWS
	std::wstring commandLine(L"CMD.exe /c \"");
	commandLine.append(_applicationPath).append(L"\"");

	OT_PROCESS_HANDLE processHandle;
	result = runApplication(_applicationPath, commandLine, processHandle, _flags);
	CloseHandle(processHandle);
#elif defined(OT_OS_LINUX)
	// TODO(linux): "CMD.exe /c" has no safe sh -c counterpart without quoting the path for the shell.
	result.setAsError(RunResult::genericErrorCode);
	result.setErrorMessage("running an application through the shell is not implemented on Linux");
#else
	assert(0); // Not implemented yet
#endif // OS_WINDOWS

	return result;
}

ot::RunResult ot::SystemProcess::runApplication(const std::string& _applicationPath, const std::string& _commandLine, OT_PROCESS_HANDLE& _processHandle, const ProcessFlags& _flags) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> strconverter;
	return runApplication(strconverter.from_bytes(_applicationPath), strconverter.from_bytes(_commandLine), _processHandle, _flags);
}

ot::RunResult ot::SystemProcess::runFileByShell(const std::string& _filePath, const std::string& _args, const std::string& _workingDir) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> strconverter;
	return runFileByShell(strconverter.from_bytes(_filePath), strconverter.from_bytes(_args), strconverter.from_bytes(_workingDir));
}

// ###########################################################################################################################################################################################################################################################################################################################

// Windows implementation

#if defined(OT_OS_WINDOWS)
ot::RunResult ot::SystemProcess::runApplication(const std::wstring& _applicationPath, const std::wstring& _commandLine, OT_PROCESS_HANDLE& _processHandle, const ProcessFlags& _flags) {
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

	if (_flags & ProcessFlag::HideWindow) {
		info.wShowWindow = SW_HIDE;
	}
	else {
		info.wShowWindow = SW_SHOW;
	}
	ZeroMemory(&processInfo, sizeof(processInfo));

	DWORD creationFlags = 0;
	if (_flags & ProcessFlag::UseUnicode) { creationFlags |= CREATE_UNICODE_ENVIRONMENT; }
	if (_flags & ProcessFlag::CreateNewConsole) { creationFlags |= CREATE_NEW_CONSOLE; }
	if (_flags & ProcessFlag::DetachedProcess) { creationFlags |= DETACHED_PROCESS; }
	if (_flags & ProcessFlag::AboveNormalPriority) { creationFlags |= ABOVE_NORMAL_PRIORITY_CLASS; }

	const bool createSuccess = CreateProcess(applicationPath, commandline, NULL, NULL, TRUE, creationFlags, penv, NULL, &info, &processInfo);
	
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

ot::RunResult ot::SystemProcess::runFileByShell(const std::wstring& _filePath, const std::wstring& _args, const std::wstring& _workingDir) {
	ot::RunResult result;

	OTAssert(!_filePath.empty(), "File path is empty");

	SHELLEXECUTEINFOW sei{};
	sei.cbSize = sizeof(sei);
	sei.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOASYNC;
	sei.lpVerb = L"open";
	sei.lpFile = static_cast<LPCWSTR>(_filePath.c_str());
	sei.lpParameters = _args.empty() ? nullptr : static_cast<LPCWSTR>(_args.c_str());
	sei.lpDirectory = _workingDir.empty() ? nullptr : static_cast<LPCWSTR>(_workingDir.c_str());
	sei.nShow = SW_SHOWNORMAL;

	if (!ShellExecuteExW(&sei)) {
		result.setAsError(GetLastError());
		result.setErrorMessage("starting a process via ShellExecuteExW failed");
	}

	if (sei.hProcess) {
		CloseHandle(sei.hProcess);
	}

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

#elif defined(OT_OS_LINUX)

namespace {
	std::vector<std::string> splitCommandLine(const std::string& _commandLine) {
		std::vector<std::string> arguments;
		std::string current;
		bool inQuotes = false;
		bool hasArgument = false;
		size_t backslashes = 0;

		for (const char c : _commandLine) {
			if (c == '\\') {
				backslashes++;
				hasArgument = true;
				continue;
			}
			if (c == '"') {
				current.append(backslashes / 2, '\\');
				if (backslashes % 2 == 1) {
					current.push_back('"');
				}
				else {
					inQuotes = !inQuotes;
				}
				backslashes = 0;
				hasArgument = true;
				continue;
			}

			current.append(backslashes, '\\');
			backslashes = 0;

			if ((c == ' ' || c == '\t') && !inQuotes) {
				if (hasArgument) {
					arguments.push_back(current);
					current.clear();
					hasArgument = false;
				}
				continue;
			}

			current.push_back(c);
			hasArgument = true;
		}

		current.append(backslashes, '\\');
		if (hasArgument) {
			arguments.push_back(current);
		}
		return arguments;
	}
}

ot::RunResult ot::SystemProcess::runApplication(const std::wstring& _applicationPath, const std::wstring& _commandLine, OT_PROCESS_HANDLE& _processHandle, const ProcessFlags& _flags) {
	ot::RunResult resultMessage;
	_processHandle = OT_INVALID_PROCESS_HANDLE;

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> strconverter;
	const std::string applicationPath = strconverter.to_bytes(_applicationPath);
	std::vector<std::string> arguments = splitCommandLine(strconverter.to_bytes(_commandLine));
	if (arguments.empty()) {
		arguments.push_back(applicationPath);
	}

	std::vector<char*> argv;
	for (std::string& argument : arguments) {
		argv.push_back(argument.data());
	}
	argv.push_back(nullptr);

	posix_spawn_file_actions_t fileActions;
	int error = posix_spawn_file_actions_init(&fileActions);
	if (error != 0) {
		resultMessage.setAsError(static_cast<uint32_t>(error));
		resultMessage.setErrorMessage("starting a process failed");
		return resultMessage;
	}

	posix_spawnattr_t attributes;
	error = posix_spawnattr_init(&attributes);
	if (error != 0) {
		posix_spawn_file_actions_destroy(&fileActions);
		resultMessage.setAsError(static_cast<uint32_t>(error));
		resultMessage.setErrorMessage("starting a process failed");
		return resultMessage;
	}

	error = posix_spawn_file_actions_addclosefrom_np(&fileActions, STDERR_FILENO + 1);

	if (error == 0 && (_flags & ProcessFlag::DetachedProcess)) {
		error = posix_spawnattr_setflags(&attributes, POSIX_SPAWN_SETSID);
	}

	pid_t processID = OT_INVALID_PROCESS_HANDLE;
	if (error == 0) {
		error = posix_spawn(&processID, applicationPath.c_str(), &fileActions, &attributes, argv.data(), environ);
	}
	posix_spawnattr_destroy(&attributes);
	posix_spawn_file_actions_destroy(&fileActions);

	if (error == 0) {
		_processHandle = processID;
	}
	else {
		resultMessage.setAsError(static_cast<uint32_t>(error));
		resultMessage.setErrorMessage("starting a process failed");
	}

	return resultMessage;
}

ot::RunResult ot::SystemProcess::runFileByShell(const std::wstring&, const std::wstring&, const std::wstring&) {
	// TODO(linux): ShellExecute opens a file with its associated program, there is no direct equivalent.
	ot::RunResult result;
	result.setAsError(RunResult::genericErrorCode);
	result.setErrorMessage("running a file by shell is not implemented on Linux");
	return result;
}

bool ot::SystemProcess::isApplicationRunning(OT_PROCESS_HANDLE& _processHandle) {
	if (_processHandle == OT_INVALID_PROCESS_HANDLE) {
		return false;
	}

	int status = 0;
	if (waitpid(_processHandle, &status, WNOHANG) == 0) {
		return true;
	}

	_processHandle = OT_INVALID_PROCESS_HANDLE;
	return false;
}

#endif
