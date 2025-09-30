#include "CSVToTableTransformer.h"
#include "OTCore/String.h"
#include "OTCore/LogDispatcher.h"
#include "OTCore/ContainerHelper.h"
#include "OTCore/VariableToStringConverter.h"

ot::GenericDataStructMatrix CSVToTableTransformer::operator()(const std::string& _csvText, const CSVProperties& _properties) {
	assert(_properties.m_columnDelimiter != "" && _properties.m_rowDelimiter != "");
	std::vector<std::string> rows = ot::ContainerHelper::toVector(ot::String::smartSplit(_csvText, _properties.m_rowDelimiter, _properties.m_evaluateEscapeCharacters, true));

	std::list<std::vector<std::string>> matrixRaw;
	int columnSize = -1;
	for (const std::string& row : rows) {
		const std::vector<std::string> columns = ot::ContainerHelper::toVector(ot::String::smartSplit(row, _properties.m_columnDelimiter, _properties.m_evaluateEscapeCharacters));
		if (columns.size() != columnSize && columnSize >= 0) {
			OT_LOG_W("Inconsistent column count");
		}
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
			if (cellValue.isConstCharPtr()) {
				if (_properties.m_evaluateEscapeCharacters) {
					csvText += ot::String::addEscapeCharacters(cellValue.getConstCharPtr());
				}
				else {
					csvText += cellValue.getConstCharPtr();
				}
				
			}			
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
