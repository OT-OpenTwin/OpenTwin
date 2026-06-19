// @otlicense
// File: FixtureTableStatisticAnalyser.cpp
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

#include "FixtureTableStatisticAnalyser.h"
#include "DataSourceHandler.h"
#include "FileHelper.h"

FixtureTableStatisticAnalyser::FixtureTableStatisticAnalyser()
	:m_filePath(FileHelper::getFilePath())
{
	std::string fullPath = m_filePath + "WorkingTestTableForAnalytics.csv";
	loadFileContent(fullPath);
}
FixtureTableStatisticAnalyser::~FixtureTableStatisticAnalyser()
{
	delete m_analyser;
	m_analyser = nullptr;
	delete m_extractor;
	m_extractor = nullptr;
}


uint64_t FixtureTableStatisticAnalyser::countRows()
{
	return m_analyser->CountRows();
}

uint64_t FixtureTableStatisticAnalyser::countColumns()
{
	return m_analyser->CountColumns();
}

int FixtureTableStatisticAnalyser::countUniquesInColumn(int _column)
{
	return m_analyser->CountUniquesInColumn(_column);
}


std::vector<size_t> FixtureTableStatisticAnalyser::countUniquesInAllColumns()
{
	return m_analyser->CountUniquesInColumns();
}

void FixtureTableStatisticAnalyser::loadFileContent(std::string _fullPath)
{
	
	auto fileContent = FileHelper::extractFileContentAsBinary(_fullPath);
	auto extractorCSV = new TableExtractorCSV();
	extractorCSV->SetFileContent(fileContent);
	m_extractor = extractorCSV;
	m_analyser = new TableStatisticAnalyser(m_extractor);
}
