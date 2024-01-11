#include <fstream>
#include "FixtureTableExtractorCSV.h"
#include "DataSourceHandler.h"
#include "FileHelper.h"

void FixtureTableExtractorCSV::LoadFileWithSemicolonRowDelimiter(std::string name)
{
	LoadFileContent(name);
	_extractor->SetRowDelimiter(';');
}



void FixtureTableExtractorCSV::LoadFileContent(std::string fullPath)
{
	auto fileContent = FileHelper::ExtractFileContentAsBinary(fullPath);
	auto extractorCSV = new TableExtractorCSV();
	extractorCSV->SetFileContent(fileContent);
	_extractor = extractorCSV;
}

void FixtureTableExtractorCSV::LoadFileWithCommaColumnDelimiter(std::string name)
{
	LoadFileContent(name);
	_extractor->SetColumnDelimiter(',');
}

void FixtureTableExtractorCSV::LoadFileWithTabColumnDelimiter(std::string name)
{
	LoadFileContent(name);
	_extractor->SetColumnDelimiter('\t');
} 

std::vector<std::string> FixtureTableExtractorCSV::GetAllRows(void)
{
	std::vector<std::string> allRows;
	while (_extractor->HasNextLine())
	{
		std::string line;
		_extractor->GetNextLine(line);
		allRows.push_back(line);
	}

	return allRows;
}

std::vector<std::string> FixtureTableExtractorCSV::GetAllColumns(void)
{
	std::vector<std::string> allColumns;
	_extractor->GetNextLine(allColumns);
	return allColumns;
}
