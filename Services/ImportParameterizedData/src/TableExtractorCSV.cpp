#include "TableExtractorCSV.h"
#include "FileToTableExtractorRegistrar.h"
#include "EntityParameterizedDataSourceCSV.h"

#include <algorithm>
#include <exception>

TableExtractorCSV::~TableExtractorCSV()
{
}

void TableExtractorCSV::SetRowDelimiter(const char delimiter)
{
	_rowDelimiter = delimiter;
}

void TableExtractorCSV::SetColumnDelimiter(const char delimiter)
{
	_columnDelimiter = delimiter;
}


void TableExtractorCSV::ExtractFromEntitySource(EntityFile* source)
{
	auto csvSource = dynamic_cast<EntityParameterizedDataSourceCSV*>(source);
	if (csvSource == nullptr)
	{
		//This class deals specifically with EntityParameterizedDataSourceCSV.
		assert(0);
	}
	_rowDelimiter =	csvSource->getRowDelimiter()[0];
	_columnDelimiter = csvSource->getColumnDelimiter()[0];
	source->loadData();
	const std::vector<char>& fileContent =	source->getData()->getData();
	//In wstring
	_fileContentStream.str(std::string(fileContent.begin(), fileContent.end()));
}

void TableExtractorCSV::SetFileContent(std::vector<char> fileContent)
{
	std::string lineAsString = std::string(fileContent.begin(), fileContent.end());
	_fileContentStream.str(lineAsString);
}

void TableExtractorCSV::GetNextLine(std::vector<std::string>& lineSegments)
{
	std::string t;
	std::getline(_fileContentStream, t, _rowDelimiter);
	Split(t, lineSegments);
}

void TableExtractorCSV::GetNextLine(std::string & entireLine)
{
	std::getline(_fileContentStream, entireLine, _rowDelimiter);
}

bool TableExtractorCSV::HasNextLine()
{
	return !_fileContentStream.eof();
}

void TableExtractorCSV::ResetIterator()
{
	_fileContentStream.clear();
	_fileContentStream.seekg(0);
}

void TableExtractorCSV::Split(std::string & line, std::vector<std::string>& lineSegments)
{
	std::string::difference_type maxColumns = std::count(line.begin(), line.end(), _columnDelimiter);
	lineSegments.reserve(maxColumns);
	
	std::string temp(""), composed("");
	std::istringstream stream(line);

	while (!stream.eof())
	{
		std::getline(stream, temp, _columnDelimiter);
		composed += temp;
		std::string::difference_type n = std::count(composed.begin(), composed.end(), '"');
		if (n % 2 == 0)
		{
			composed.erase(remove(composed.begin(), composed.end(), '"'), composed.end());
			lineSegments.push_back(composed);
			composed = "";
		}
		else
		{
			if (!stream.eof())
			{
				composed += _columnDelimiter;
			}
		}
	}
	if (composed != "")
	{
		composed.erase(remove(composed.begin(), composed.end(), '"'), composed.end());
		lineSegments.push_back(composed);
	}

	lineSegments.shrink_to_fit();
}

static FileToTableExtractorRegistrar<TableExtractorCSV> textFiles("txt");
static FileToTableExtractorRegistrar<TableExtractorCSV> csvFiles("csv");
static FileToTableExtractorRegistrar<TableExtractorCSV> CSVFiles("CSV");
