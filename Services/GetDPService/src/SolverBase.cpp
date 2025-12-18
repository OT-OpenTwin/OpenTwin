// @otlicense
// File: SolverBase.cpp
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

#include "SolverBase.h"

#include "OTServiceFoundation/UiComponent.h"

#include <windows.h> // winapi
#include <UserEnv.h>
#include <codecvt>
#include <stdlib.h>

void SolverBase::setData(EntityBase* _solverEntity, const std::string& _meshDataName, std::map<ot::UID, ot::EntityInformation>& _meshItemInfo,
						 std::map<ot::UID, EntityProperties>& _entityProperties, std::map<std::string, size_t>& _groupNameToIdMap,
						 std::map<std::string, EntityProperties>& _materialProperties)
{
	solverEntity = _solverEntity;
	meshDataName = _meshDataName;
	meshItemInfo = _meshItemInfo;
	entityProperties = _entityProperties;
	groupNameToIdMap = _groupNameToIdMap;
	materialProperties = _materialProperties;
}

bool SolverBase::isPECMaterial(const std::string& materialName)
{
	EntityPropertiesSelection* materialType = dynamic_cast<EntityPropertiesSelection*>(materialProperties[materialName].getProperty("Material type"));
	
	if (materialType != nullptr)
	{
		return (materialType->getValue() == "PEC");
	}

	return false;
}

void SolverBase::runSolverExe(const std::string& inputFileName, const std::string& solvTarget, const std::string& postTarget, const std::string& workingDirectory, ot::components::UiComponent* uiComponent)
{
	std::string exePath = readEnvironmentVariable("OPENTWIN_DEV_ROOT");
	if (exePath.empty())
	{
		// Get the path of the executable
#ifdef _WIN32
		char path[MAX_PATH] = { 0 };
		GetModuleFileNameA(NULL, path, MAX_PATH);
		exePath = path;
#else
		char result[PATH_MAX];
		ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
		ca_cert_flile = std::string(result, (count > 0) ? count : 0);
#endif
		exePath = exePath.substr(0, exePath.rfind('\\'));
	}
	else
	{
		exePath += "\\Deployment";
	}

	std::string commandLine = "\"" + exePath + "\\getdp.exe\"" + " model -solve " + solvTarget + " -post " + postTarget;

	if (!runExecutableAndWaitForCompletion(commandLine, workingDirectory, uiComponent))
	{
		throw std::string("ERROR: Unable to run GETDP executable. Command line: " + commandLine);
	}
}

#pragma comment(lib, "userenv.lib")

bool SolverBase::runExecutableAndWaitForCompletion(std::string commandLine, std::string workingDirectory, ot::components::UiComponent* uiComponent)
{
	HANDLE hToken = NULL;
	BOOL ok = OpenProcessToken(GetCurrentProcess(), TOKEN_READ, &hToken);
	assert(ok);

	wchar_t* penv = nullptr;
	ok = CreateEnvironmentBlock((void**)&penv, hToken, TRUE);
	assert(ok);

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> strconverter;
	std::wstring wideString = strconverter.from_bytes(commandLine);

	LPWSTR cl = new WCHAR[wideString.size() + 1];
	wcscpy_s(cl, wideString.size() + 1, wideString.c_str());

	std::wstring workDir = strconverter.from_bytes(workingDirectory);
	LPWSTR wd = new WCHAR[workDir.size() + 1];
	wcscpy_s(wd, workDir.size() + 1, workDir.c_str());

	STARTUPINFO info;
	PROCESS_INFORMATION processInfo;

	HANDLE g_hChildStd_OUT_Rd = NULL;
	HANDLE g_hChildStd_OUT_Wr = NULL;

	SECURITY_ATTRIBUTES saAttr;

	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	bool success = true;

	if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0))
	{
		success = false;
	}
	else
	{
		if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
		{
			CloseHandle(g_hChildStd_OUT_Wr);
			success = false;
		}
		else
		{
			ZeroMemory(&info, sizeof(info));
			info.cb = sizeof(info);
			info.hStdError = g_hChildStd_OUT_Wr;
			info.hStdOutput = g_hChildStd_OUT_Wr;
			info.dwFlags |= (STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW);
			info.wShowWindow = SW_HIDE;

			ZeroMemory(&processInfo, sizeof(processInfo));

			if (CreateProcess(NULL, cl, NULL, NULL, TRUE, CREATE_UNICODE_ENVIRONMENT | ABOVE_NORMAL_PRIORITY_CLASS, penv, wd, &info, &processInfo))
			{
				CloseHandle(g_hChildStd_OUT_Wr);
				readFromPipe(g_hChildStd_OUT_Rd, uiComponent);

				WaitForSingleObject(processInfo.hProcess, INFINITE);
				CloseHandle(processInfo.hProcess);
				CloseHandle(processInfo.hThread);
			}
			else
			{
				CloseHandle(g_hChildStd_OUT_Wr);
				success = false;
			}
		}
	}

	delete[] cl;
	cl = nullptr;

	DestroyEnvironmentBlock(penv);

	return success;
}

void SolverBase::readFromPipe(HANDLE g_hChildStd_OUT_Rd, ot::components::UiComponent* uiComponent)

// Read output from the child process's pipe for STDOUT
// and write to the parent process's pipe for STDOUT. 
// Stop when there is no more data. 
{
	#define BUFSIZE 4096 

	DWORD dwRead;
	CHAR chBuf[BUFSIZE];
	BOOL bSuccess = FALSE;

	std::string lastProgressText;

	for (;;)
	{
		bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
		if (!bSuccess || dwRead == 0) break;

		std::string text(chBuf, dwRead);

		normalizeLineEndings(text);

		updateSolverLogAndProgress(text, uiComponent, lastProgressText);

		if (!bSuccess) break;
	}
}

void SolverBase::normalizeLineEndings(std::string& text)
{
	std::string normalized;
	normalized.reserve(text.size());

	for (std::size_t i = 0; i < text.size(); ++i)
	{
		if (text[i] == '\r')
		{
			// Convert '\r\n' or '\r' to '\n'
			normalized.push_back('\n');

			// Skip '\n' if this is a Windows line ending
			if (i + 1 < text.size() && text[i + 1] == '\n')
				++i;
		}
		else
		{
			normalized.push_back(text[i]);
		}
	}

	text.swap(normalized);
}

std::optional<SolverBaseParsedLine> SolverBase::parseLine(const std::string& line)
{
	std::size_t i = 0;

	// Skip leading whitespace
	while (i < line.size() && std::isspace(static_cast<unsigned char>(line[i])))
		++i;

	// Parse the numeric value
	if (i >= line.size() || !std::isdigit(static_cast<unsigned char>(line[i])))
		return std::nullopt;

	int value = 0;
	while (i < line.size() && std::isdigit(static_cast<unsigned char>(line[i])))
	{
		value = value * 10 + (line[i] - '0');
		++i;
	}

	// Expect a percent sign
	if (i >= line.size() || line[i] != '%')
		return std::nullopt;

	// Find the colon after the percent sign
	auto colonPos = line.find(':', i + 1);
	if (colonPos == std::string::npos)
		return std::nullopt;

	// Extract the text after the colon
	std::string text = line.substr(colonPos + 1);

	// Optionally trim leading whitespace after the colon
	std::size_t start = 0;
	while (start < text.size() && std::isspace(static_cast<unsigned char>(text[start])))
		++start;

	text.erase(0, start);

	return SolverBaseParsedLine{ value, text };
}


void SolverBase::updateSolverLogAndProgress(const std::string &text, ot::components::UiComponent* uiComponent, std::string &lastProgressText)
{
	std::istringstream input(text);
	std::ostringstream output;

	std::string line;

	while (std::getline(input, line))
	{
		if (auto parsed = parseLine(line))
		{
			if (parsed->textAfterColon != lastProgressText)
			{
				uiComponent->setProgressInformation(parsed->textAfterColon, false);
				lastProgressText = parsed->textAfterColon;
			}

			uiComponent->setProgress(parsed->number);
		}
		else
		{
			output << line << '\n';
		}
	}

	uiComponent->displayMessage(output.str());
	solverOutput << output.str();
}

std::string SolverBase::readEnvironmentVariable(const std::string& variableName)
{
	std::string variableValue;

	const int nSize = 32767;
	char* buffer = new char[nSize];

	if (GetEnvironmentVariableA(variableName.c_str(), buffer, nSize))
	{
		variableValue = buffer;
	}

	return variableValue;
}
