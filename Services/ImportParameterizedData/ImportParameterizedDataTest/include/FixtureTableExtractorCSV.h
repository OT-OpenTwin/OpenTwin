// @otlicense

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
