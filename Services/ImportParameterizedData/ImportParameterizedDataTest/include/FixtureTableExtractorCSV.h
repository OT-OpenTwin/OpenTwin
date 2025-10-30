// @otlicense
// File: FixtureTableExtractorCSV.h
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

#pragma once
#include <gtest/gtest.h>
#include "TableExtractorCSV.h"

class FixtureTableExtractorCSV : public testing::TestWithParam<int>
{
public:
	FixtureTableExtractorCSV();

	~FixtureTableExtractorCSV() 
	{ 
		if (m_extractor != nullptr)
		{
			delete m_extractor;
			m_extractor = nullptr;
		}
	}
	
	const std::string getFilePath() { return m_filePath; }
	void loadFileContent(std::string fullPath);
	void loadFileWithSemicolonRowDelimiter(std::string name);
	void loadFileWithCommaColumnDelimiter(std::string name);
	void loadFileWithTabColumnDelimiter(std::string name);
	std::vector<std::string> getAllRows(void);
	std::vector<std::string> getAllColumns(void);

private:
	TableExtractorCSV* m_extractor = nullptr;
	const std::string m_filePath;
};
