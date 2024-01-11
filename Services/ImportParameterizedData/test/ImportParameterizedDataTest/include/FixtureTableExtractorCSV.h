#pragma once
#include <gtest/gtest.h>
#include "TableExtractorCSV.h"

class FixtureTableExtractorCSV : public testing::TestWithParam<int>
{
public:
	~FixtureTableExtractorCSV() 
	{ 
		if (_extractor != nullptr)
		{
			delete _extractor;
			_extractor = nullptr;
		}
	}
	
	const std::string GetFilePath() { return filePath; }
	void LoadFileContent(std::string fullPath);
	void LoadFileWithSemicolonRowDelimiter(std::string name);
	void LoadFileWithCommaColumnDelimiter(std::string name);
	void LoadFileWithTabColumnDelimiter(std::string name);
	std::vector<std::string> GetAllRows(void);
	std::vector<std::string> GetAllColumns(void);

private:
	TableExtractorCSV* _extractor = nullptr;
	const std::string filePath = "C:\\SimulationPlatform\\Libraries\\ImportParameterizedData\\test\\ImportParameterizedDataTest\\Files\\";
};
