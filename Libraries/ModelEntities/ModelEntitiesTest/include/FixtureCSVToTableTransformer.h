#pragma once
#include <gtest/gtest.h>
#include "CSVToTableTransformer.h"

class FixtureCSVToTableTransformer : public testing::TestWithParam<int>
{
public:
	FixtureCSVToTableTransformer();
	const std::string getFilePath() { return m_filePath; }
	std::string loadFileContent(std::string fullPath);
	ot::GenericDataStructMatrix getTable(const std::string& text, CSVProperties& properties);
private:
	CSVToTableTransformer m_extractor;
	const std::string m_filePath;
};
