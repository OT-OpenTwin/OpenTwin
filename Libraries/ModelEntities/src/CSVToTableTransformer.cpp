#include "CSVToTableTransformer.h"

ot::GenericDataStructMatrix CSVToTableTransformer::operator()(const std::string& _csvText, const CSVProperties& _properties) const
{
	std::vector<std::string> rows = splitByDelimiter(_csvText, _properties.m_rowDelimiter);

	size_t maxNumberOfColumns(0);
	for (std::string& row : rows)
	{
		size_t  numberOfColumns = countStringInString(row, _properties.m_columnDelimiter);
		maxNumberOfColumns = maxNumberOfColumns > numberOfColumns ? maxNumberOfColumns : numberOfColumns;
	}

	ot::MatrixEntryPointer matrixDimensions;
	matrixDimensions.m_column = maxNumberOfColumns;
	matrixDimensions.m_row = rows.size();
	ot::GenericDataStructMatrix matrix(matrixDimensions);

	ot::MatrixEntryPointer entryPointer;
	entryPointer.m_column = 0;
	entryPointer.m_row = 0;

	for (std::string& row : rows)
	{
		std::vector<std::string> entries = splitByDelimiter(row, _properties.m_columnDelimiter);

		for (std::string& entry : entries)
		{
			matrix.setValue(entryPointer, ot::Variable(entry));
			entryPointer.m_column++;
		}
		entryPointer.m_column = 0;
		entryPointer.m_row++;
	}
	return matrix;
}

std::vector<std::string> CSVToTableTransformer::splitByDelimiter(const std::string& _text, const std::string& _delimiter) const
{
	size_t pos_start = 0, pos_end, delim_len = _delimiter.length();
	std::vector<std::string> segments;
	size_t numberOfSegments = countStringInString(_text, _delimiter);
	segments.reserve(numberOfSegments);

	while ((pos_end = _text.find(_delimiter, pos_start)) != std::string::npos)
	{
		std::string row = _text.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		segments.push_back(row);
	}
	return segments;
}

uint64_t CSVToTableTransformer::countStringInString(const std::string& _text, const std::string& _searchCriteria) const
{
	uint64_t count = 0;
	uint64_t nPos = _text.find(_searchCriteria, 0);
	while (nPos != std::string::npos)
	{
		count++;
		nPos = _text.find(_searchCriteria, nPos + 1);
	}
	return count;
}
