// @otlicense

#include <fstream>
#include "OTSystem/OperatingSystem.h"
#include "FixtureTableExtractorCSV.h"
#include "DataSourceHandler.h"
#include "FileHelper.h"

void FixtureTableExtractorCSV::loadFileWithSemicolonRowDelimiter(std::string name)
{
	loadFileContent(name);
	m_extractor->SetRowDelimiter(';');
}

FixtureTableExtractorCSV::FixtureTableExtractorCSV()
	: m_filePath (FileHelper::getFilePath())
{

}

void FixtureTableExtractorCSV::loadFileContent(std::string fullPath)
{
	auto fileContent = FileHelper::extractFileContentAsBinary(fullPath);
	auto extractorCSV = new TableExtractorCSV();
	extractorCSV->SetFileContent(fileContent);
	m_extractor = extractorCSV;
}

void FixtureTableExtractorCSV::loadFileWithCommaColumnDelimiter(std::string name)
{
	loadFileContent(name);
	m_extractor->SetColumnDelimiter(',');
}

void FixtureTableExtractorCSV::loadFileWithTabColumnDelimiter(std::string name)
{
	loadFileContent(name);
	m_extractor->SetColumnDelimiter('\t');
} 

std::vector<std::string> FixtureTableExtractorCSV::getAllRows(void)
{
	std::vector<std::string> allRows;
	while (m_extractor->HasNextLine())
	{
		std::string line;
		m_extractor->GetNextLine(line);
		allRows.push_back(line);
	}

	return allRows;
}

std::vector<std::string> FixtureTableExtractorCSV::getAllColumns(void)
{
	std::vector<std::string> allColumns;
	m_extractor->GetNextLine(allColumns);
	return allColumns;
}
