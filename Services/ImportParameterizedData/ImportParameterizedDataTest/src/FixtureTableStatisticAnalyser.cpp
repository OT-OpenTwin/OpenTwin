// @otlicense

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
