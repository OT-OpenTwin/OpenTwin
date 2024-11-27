#include "CSVToTableTransformer.h"
#include "OTCore/VariableToStringConverter.h"

ot::GenericDataStructMatrix CSVToTableTransformer::operator()(const std::string& _csvText, const CSVProperties& _properties) {
	assert(_properties.m_columnDelimiter != "" && _properties.m_rowDelimiter != "");
	std::vector<std::string> rows = splitByDelimiter(_csvText, _properties.m_rowDelimiter);

	std::list<std::vector<std::string>> matrixRaw;
		
	for (uint64_t i = 0; i < rows.size(); i++)
	{
		std::string& row = rows[i];
		const std::vector<std::string> columns = splitByDelimiter(row, _properties.m_columnDelimiter);
		matrixRaw.push_back(columns);
	}

	const ot::GenericDataStructMatrix matrix = std::move(transformRawMatrixToGenericDatastruct(matrixRaw));

	return matrix;
}

std::string CSVToTableTransformer::operator()(const ot::GenericDataStructMatrix& _matrix, const CSVProperties& _properties) {
	ot::MatrixEntryPointer matrixEntry;
	uint32_t numberOfColumns =	_matrix.getNumberOfColumns();
	uint32_t numberOfRows = _matrix.getNumberOfRows();
	std::string csvText("");
	for (matrixEntry.m_row = 0; matrixEntry.m_row < numberOfRows; matrixEntry.m_row++)
	{
		for (matrixEntry.m_column = 0; matrixEntry.m_column < numberOfColumns; matrixEntry.m_column++)
		{
			const ot::Variable& cellValue =	_matrix.getValue(matrixEntry);
			assert(cellValue.isConstCharPtr());
			csvText += cellValue.getConstCharPtr();
			if (matrixEntry.m_column < numberOfColumns - 1)
			{
				csvText += _properties.m_columnDelimiter;
			}
		}
		if (matrixEntry.m_row< numberOfRows - 1)
		{
			csvText += _properties.m_rowDelimiter;
		}
	}
	return csvText;
}

void CSVToTableTransformer::clearBuffer()
{
	m_composed = "";
	m_segments.clear();
}

void CSVToTableTransformer::buildSegment(const std::string& _subString, const std::string& _delimiter, bool _lastSegment)
{
	m_composed += _subString;
	std::string::difference_type numberOfMasks = std::count(m_composed.begin(), m_composed.end(), m_maskingChar);
	if (numberOfMasks % 2 == 0)
	{
		if (_lastSegment)
		{
			m_segments.push_back(m_composed);
			m_composed = "";
		}
	}
	else
	{
		if (_lastSegment)
		{
			m_composed += _delimiter;
		}
	}
}

std::vector<std::string> CSVToTableTransformer::splitByDelimiter(const std::string& _text, const std::string& _delimiter)
{
	clearBuffer();
	size_t pos_start = 0, pos_end, delim_len = _delimiter.length();
	size_t numberOfSegments = countStringInString(_text, _delimiter);
	m_segments.reserve(numberOfSegments + 1);
	std::string composed("");
	bool lastEntry = false;
	while ((pos_end = _text.find(_delimiter, pos_start)) != std::string::npos)
	{
		const std::string row = _text.substr(pos_start, pos_end - pos_start);
		buildSegment(row,_delimiter);
		pos_start = pos_end + delim_len;
	}
	if (pos_start < _text.size())
	{
		std::string row = _text.substr(pos_start);
		buildSegment(row, _delimiter,false);
		m_segments.push_back(m_composed);
	}
	m_segments.shrink_to_fit();
	std::vector<std::string> returnValue(m_segments);
	clearBuffer();
	return returnValue;
}

ot::GenericDataStructMatrix CSVToTableTransformer::transformRawMatrixToGenericDatastruct(const std::list<std::vector<std::string>>& _rawMatrix)
{
	ot::MatrixEntryPointer matrixDimensions;

	size_t maxNumberOfColumns(1);
	for (auto& row : _rawMatrix)
	{
		size_t  numberOfColumns = row.size();
		maxNumberOfColumns = maxNumberOfColumns > numberOfColumns ? maxNumberOfColumns : numberOfColumns;
	}
	matrixDimensions.m_column = maxNumberOfColumns;
	matrixDimensions.m_row = _rawMatrix.size();
	ot::GenericDataStructMatrix matrix(matrixDimensions);

	ot::MatrixEntryPointer entryPointer;
	auto rowPointer = _rawMatrix.begin();
	for (entryPointer.m_row = 0; entryPointer.m_row < _rawMatrix.size(); entryPointer.m_row++, rowPointer++)
	{
		for (entryPointer.m_column = 0; entryPointer.m_column < rowPointer->size(); entryPointer.m_column++)
		{
			std::string rawValue = (*rowPointer)[entryPointer.m_column];
			//rawValue.erase(remove(rawValue.begin(), rawValue.end(), m_maskingChar), rawValue.end());
			ot::Variable cellValue(rawValue);
			matrix.setValue(entryPointer, cellValue);
		}
	}

	return matrix;
}

uint64_t CSVToTableTransformer::countStringInString(const std::string& _text, const std::string& _searchCriteria) 
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
