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
