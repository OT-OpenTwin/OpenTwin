// @otlicense
// File: FixtureTextEncoding.cpp
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

#include "FixtureTextEncoding.h"
#include <fstream>

std::vector<char> FixtureTextEncoding::ReadFile(ot::TextEncoding::EncodingStandard encoding)
{
	std::string projectDir = PROJECT_DIR;
	projectDir += "TestEncodingTypeFiles\\";
	if (encoding == ot::TextEncoding::ANSI)
	{
		projectDir += "ANSI.txt";
	}
	else if (encoding == ot::TextEncoding::UTF8)
	{
		projectDir += "UTF8.txt";
	}
	else if (encoding == ot::TextEncoding::UTF8_BOM)
	{
		projectDir += "UTF8_BOM.txt";
	}
	else if (encoding == ot::TextEncoding::UTF16_BEBOM)
	{
		projectDir += "UTF16_BE_BOM.txt";
	}

	else if (encoding == ot::TextEncoding::UTF16_LEBOM)
	{
		projectDir += "UTF16_LE_BOM.txt";
	}
	else
	{
		return std::vector<char>();
	}
	
	std::ifstream file(projectDir, std::ios::binary | std::ios::ate);

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
		throw std::exception("File could not be opened");
	}
}
