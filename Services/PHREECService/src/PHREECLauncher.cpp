// @otlicense
// File: PHREECLauncher.cpp
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


#include "PHREECLauncher.h"
#include "PHREECMeshExport.h"

#include "DataBase.h"
#include "Application.h"

#include <windows.h> // winapi

#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <direct.h>
#include <tchar.h> // _tcscpy,_tcscat,_tcscmp
#include <algorithm> // replace
#include <codecvt>
#include <stdlib.h>
#include <UserEnv.h>

PHREECLauncher::PHREECLauncher(Application *app)
	: application(app)
{

}

PHREECLauncher::~PHREECLauncher()
{

}

std::string PHREECLauncher::startSolver(const std::string &dataBaseURL, const std::string &frequencyHz, const std::string &uiUrl, const std::string &projectName, const std::string &modelStateVersion, unsigned long long meshEntityID, bool debugFlag, int serviceID, int sessionCount, ot::components::ModelComponent *modelComponent)
{
	// Find the name of the temporary directory where we run the solver and create the directory
	std::string tempDirPath = getUniqueTempDir();
	if (tempDirPath.empty()) return "ERROR: Unable to create temporary working directory (TMP environment variable needs to be set)";

	_mkdir(tempDirPath.c_str());

	// Extract the mesh into the temp folder

	std::string outputFileBase = tempDirPath + "\\model";

	PHREECMeshExport meshExport(application);
	std::string meshResult = meshExport.exportMeshData(dataBaseURL, projectName, meshEntityID, outputFileBase, modelComponent);
	if (!meshResult.empty()) return "ERROR: The specified mesh is not a valid first order tetrahedral mesh";

	// Run the solver in the temp folder
	std::string solverResult = runPHREECSolver(outputFileBase + ".pmd", tempDirPath, frequencyHz, uiUrl);
	if (!solverResult.empty()) return solverResult;

	// Read the output file
	std::string outputText = readOutputFile(outputFileBase + ".info");
	if (outputText.empty()) return "ERROR: Unable to read solver output from file: " + outputFileBase + ".info";

	// Remove the temp dir
	if (debugFlag)
	{
		outputText += "\n\nWARNING: The working folder has not been deleted for debugging purposes: " + tempDirPath;
	}
	else
	{
		if (!deleteDirectory(tempDirPath)) return "ERROR: Unable to remove the temporary working directory: " + tempDirPath;
	}

	// return the result string
	return outputText;
}

std::string PHREECLauncher::readOutputFile(const std::string &fileName)
{
	std::ifstream outputFile(fileName);

	std::stringstream content;
	std::string line;

	for (std::string line; std::getline(outputFile, line); ) 
	{
		content << line << std::endl;
	}

	return content.str();
}

std::string PHREECLauncher::getUniqueTempDir(void)
{
	std::string tempDir = getSystemTempDir();

	size_t count = 1;
	std::string uniqueTempDir;

	do
	{
		uniqueTempDir = tempDir + "\\PHREEC_WORK" + std::to_string(count);
		count++;

	} while (checkFileOrDirExists(uniqueTempDir));

	return uniqueTempDir;
}

std::string PHREECLauncher::getSystemTempDir(void)
{
	return readEnvironmentVariable("TMP");
}

std::string PHREECLauncher::readEnvironmentVariable(const std::string &variableName)
{
	std::string variableValue;

	const int nSize = 32767;
	char *buffer = new char[nSize];

	if (GetEnvironmentVariableA(variableName.c_str(), buffer, nSize))
	{
		variableValue = buffer;
	}

	return variableValue;
}

bool PHREECLauncher::checkFileOrDirExists(const std::string &path)
{
	struct stat info;

	if (stat(path.c_str(), &info) != 0)
		return false;
	else if (info.st_mode & S_IFDIR)
		return true;   // This is a directoy
	else
		return true;   // This might be a file
}

bool PHREECLauncher::deleteDirectory(const std::string &pathName)
{
	// First delete all files in the directoy
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind;

	std::string fileNamePattern = pathName + "\\*";

	hFind = FindFirstFileA(fileNamePattern.c_str(), &FindFileData);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		std::string fileName = FindFileData.cFileName;
		if (fileName != "." && fileName != "..")
		{
			fileName = pathName + "\\" + fileName;
			if (!DeleteFileA(fileName.c_str())) std::cout << "ERROR: Unable to delete file: " << fileName << std::endl;
		}

		while (FindNextFileA(hFind, &FindFileData))
		{
			std::string fileName = FindFileData.cFileName;
			if (fileName != "." && fileName != "..")
			{
				fileName = pathName + "\\" + fileName;
				if (!DeleteFileA(fileName.c_str())) std::cout << "ERROR: Unable to delete file: " << fileName << std::endl;
			}
		}
		
		FindClose(hFind);
	}

	// Now remove the empty directory
	bool success = RemoveDirectoryA(pathName.c_str());
	return success;
}

std::string PHREECLauncher::runPHREECSolver(const std::string &pmdFileName, const std::string &outputDirectory, const std::string &frequencyHz, const std::string &uiUrl)
{
	std::string phreecPath = readEnvironmentVariable("PHREEC_PATH");
	if (phreecPath.empty()) return "ERROR: Unable to read PHREEC_PATH environment variable";

	std::string commandLine = "\"" + phreecPath + "\\phreec2.bat\"" + " -e \"['R','L']\" -f " + frequencyHz + " -o \"" + outputDirectory + "\" -ui \"" + uiUrl + "\" \"" + pmdFileName+ "\"";
	std::cout << commandLine << std::endl;

	if (!runExecutableAndWaitForCompletion(commandLine))
	{
		return "ERROR: Unable to run PHREEC executable. Command line: " + commandLine;
	}

	return "";
}

#pragma comment(lib, "userenv.lib")

bool PHREECLauncher::runExecutableAndWaitForCompletion(std::string commandLine)
{
	commandLine = "CMD.exe /c \"" + commandLine + "\"";

	HANDLE hToken = NULL;
	BOOL ok = OpenProcessToken(GetCurrentProcess(), TOKEN_READ, &hToken);
	assert(ok);

	wchar_t* penv = nullptr;
	ok = CreateEnvironmentBlock((void**)&penv, hToken, TRUE);
	assert(ok);

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> strconverter;
	std::wstring wideString = strconverter.from_bytes(commandLine);

	LPWSTR cl = new WCHAR[wideString.size() + 1];
	wcscpy_s(cl, wideString.size()+1, wideString.c_str());

	STARTUPINFO info;
	PROCESS_INFORMATION processInfo;

	ZeroMemory(&info, sizeof(info));
	info.cb = sizeof(info);
	ZeroMemory(&processInfo, sizeof(processInfo));

	bool success = true;

	if (CreateProcess(NULL, cl, NULL, NULL, TRUE, CREATE_UNICODE_ENVIRONMENT | ABOVE_NORMAL_PRIORITY_CLASS, penv, NULL, &info, &processInfo))
	{
		WaitForSingleObject(processInfo.hProcess, INFINITE);
		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
	}
	else
	{ 
		success = false;
	}

	delete[] cl;
	cl = nullptr;

	DestroyEnvironmentBlock(penv);

	return success;
}