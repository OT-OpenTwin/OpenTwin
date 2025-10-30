// @otlicense
// File: TmpFileManager.cpp
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

#include "TmpFileManager.h"

#include <windows.h> // winapi
#include <direct.h>

#include <iostream>

std::string TmpFileManager::createTmpDir(void)
{
	std::string tempDir = readEnvironmentVariable("TMP");

	size_t count = 1;
	std::string uniqueTempDir;

	do
	{
		uniqueTempDir = tempDir + "\\OT_TEMP_" + std::to_string(count);
		count++;
	} while (checkFileOrDirExists(uniqueTempDir));

	_mkdir(uniqueTempDir.c_str());

	return uniqueTempDir;
}

bool TmpFileManager::checkFileOrDirExists(const std::string &path)
{
	struct stat info;

	if (stat(path.c_str(), &info) != 0)
		return false;
	else if (info.st_mode & S_IFDIR)
		return true;   // This is a directoy
	else
		return true;   // This might be a file
}

std::string TmpFileManager::readEnvironmentVariable(const std::string &variableName)
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

bool TmpFileManager::deleteDirectory(const std::string &pathName)
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

