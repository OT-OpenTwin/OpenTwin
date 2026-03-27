// @otlicense
// File: CSVToTableTransformer.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#include "OTModelEntities/CSVToTableTransformer.h"
#include "OTCore/String.h"
#include "OTCore/Logging/LogDispatcher.h"
#include "OTCore/ContainerHelper.h"
#include "OTCore/Variable/VariableToStringConverter.h"

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
	matrixEntry.setRow(0);
	for (; matrixEntry.getRow() < numberOfRows; matrixEntry.moveRow())
	{
		matrixEntry.setColumn(0);
		for (; matrixEntry.getColumn() < numberOfColumns; matrixEntry.moveColumn())
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
			if (matrixEntry.getColumn() < numberOfColumns - 1)
			{
				csvText += _properties.m_columnDelimiter;
			}
		}
		if (matrixEntry.getRow() < numberOfRows - 1)
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
	matrixDimensions.setColumn((uint32_t)maxNumberOfColumns);
	matrixDimensions.setRow((uint32_t)_rawMatrix.size());
	ot::GenericDataStructMatrix matrix(matrixDimensions);

	ot::MatrixEntryPointer entryPointer;
	auto rowPointer = _rawMatrix.begin();
	entryPointer.setRow(0);
	for (; entryPointer.getRow() < _rawMatrix.size() && rowPointer != _rawMatrix.end(); entryPointer.moveRow(), rowPointer++)
	{
		entryPointer.setColumn(0);
		for (; entryPointer.getColumn() < rowPointer->size(); entryPointer.moveColumn())
		{
			std::string rawValue = (*rowPointer)[entryPointer.getColumn()];
			//rawValue.erase(remove(rawValue.begin(), rawValue.end(), m_maskingChar), rawValue.end());
			ot::Variable cellValue(rawValue);
			matrix.setValue(entryPointer, cellValue);
		}
	}

	return matrix;
}
