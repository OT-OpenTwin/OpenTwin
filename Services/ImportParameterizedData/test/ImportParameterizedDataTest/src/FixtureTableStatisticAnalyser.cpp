#include "FixtureTableStatisticAnalyser.h"
#include "DataSourceHandler.h"
#include "FileHelper.h"

FixtureTableStatisticAnalyser::FixtureTableStatisticAnalyser()
{
	std::string fullPath = filePath + "WorkingTestTableForAnalytics.csv";
	LoadFileContent(fullPath);
}
FixtureTableStatisticAnalyser::~FixtureTableStatisticAnalyser()
{
	delete _analyser;
	_analyser = nullptr;
	delete _extractor;
	_extractor = nullptr;
}


uint64_t FixtureTableStatisticAnalyser::CountRows()
{
	return _analyser->CountRows();
}

uint64_t FixtureTableStatisticAnalyser::CountColumns()
{
	return _analyser->CountColumns();
}

int FixtureTableStatisticAnalyser::CountUniquesInColumn(int column)
{
	return _analyser->CountUniquesInColumn(column);
}


std::vector<size_t> FixtureTableStatisticAnalyser::CountUniquesInAllColumns()
{
	return _analyser->CountUniquesInColumns();
}

void FixtureTableStatisticAnalyser::LoadFileContent(std::string fullPath)
{
	
	auto fileContent = FileHelper::ExtractFileContentAsBinary(fullPath);
	auto extractorCSV = new TableExtractorCSV();
	extractorCSV->SetFileContent(fileContent);
	_extractor = extractorCSV;
	_analyser = new TableStatisticAnalyser(_extractor);
}
