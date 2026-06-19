// @otlicense
// File: FixtureTableStatisticAnalyser.h
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
#include "TableStatisticAnalyser.h"
#include "TableExtractorCSV.h"

class FixtureTableStatisticAnalyser : public testing::TestWithParam<int>
{
public:
	FixtureTableStatisticAnalyser();
	~FixtureTableStatisticAnalyser();
	uint64_t countRows();
	uint64_t countColumns();
	int countUniquesInColumn(int column);
	std::vector<size_t> countUniquesInAllColumns();
	
private:
	TableExtractor * m_extractor = nullptr;
	TableStatisticAnalyser * m_analyser = nullptr;
	const std::string m_filePath;
	void loadFileContent(std::string _fullPath);
};
