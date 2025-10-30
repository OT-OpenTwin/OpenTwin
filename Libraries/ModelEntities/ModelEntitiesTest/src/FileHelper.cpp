// @otlicense
// File: FileHelper.cpp
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

#include "FileHelper.h"
#include "OTSystem/OperatingSystem.h"

#include <fstream>


std::vector<char> FileHelper::extractFileContentAsBinary(const std::string& fileName)
{
	std::ifstream file(fileName, std::ios::binary | std::ios::ate);

	if (file.is_open())
	{
		std::streampos size = file.tellg();

		std::shared_ptr<char> memBlock(new char[size]);
		file.seekg(0, std::ios::beg);
		file.read(memBlock.get(), size);
		file.close();
		std::vector<char> memBlockVector(memBlock.get(), memBlock.get() + size);
		return memBlockVector;
	}
	else
	{
		throw std::exception("File cannot be opened.");
	}
}

std::string FileHelper::getFilePath()
{
	std::string executablePath = ot::OperatingSystem::getCurrentExecutableDirectory();
	executablePath = executablePath.substr(0, executablePath.find_last_of("\\/")); //One level up
	executablePath = executablePath.substr(0, executablePath.find_last_of("\\/")); // Other level up
	const std::string filePath = executablePath + "\\ModelEntitiesTest\\Files\\";
	return filePath;
}
