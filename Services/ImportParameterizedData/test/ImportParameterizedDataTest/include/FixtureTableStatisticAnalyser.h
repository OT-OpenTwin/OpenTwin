#pragma once
#include "gtest/gtest.h"
#include "TableStatisticAnalyser.h"
#include "TableExtractorCSV.h"

class FixtureTableStatisticAnalyser : public testing::TestWithParam<int>
{
public:
	FixtureTableStatisticAnalyser();
	~FixtureTableStatisticAnalyser();
	uint64_t CountRows();
	uint64_t CountColumns();
	int CountUniquesInColumn(int column);
	std::vector<size_t> CountUniquesInAllColumns();
	
private:
	TableExtractor * _extractor = nullptr;
	TableStatisticAnalyser * _analyser = nullptr;
	const std::string filePath = "C:\\SimulationPlatform\\Libraries\\ImportParameterizedData\\test\\ImportParameterizedDataTest\\Files\\";
	void LoadFileContent(std::string fullPath);
};
