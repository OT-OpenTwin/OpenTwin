// @otlicense
// File: StringConverter.cpp
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

#include "StringConverter.h"

std::string ot::toString(const ot::GenericDataStruct* _dataStruct)
{
	ot::VariableToStringConverter converter;
	std::string dataStructAsString("");
	const ot::GenericDataStructSingle* single = dynamic_cast<const ot::GenericDataStructSingle*>(_dataStruct);
	if (single != nullptr)
	{
		const std::string stringVal = converter(single->getValue());
		dataStructAsString = stringVal + "\n";
	}

	const ot::GenericDataStructVector* vector = dynamic_cast<const ot::GenericDataStructVector*>(_dataStruct);
	if (vector != nullptr)
	{
		dataStructAsString = "[";
		const std::vector<ot::Variable>& values = vector->getValues();
		const uint32_t numberOfEntries = vector->getNumberOfEntries();
		for (uint32_t index = 0; index < numberOfEntries; index++)
		{
			const std::string stringVal = converter(values[index]);
			dataStructAsString += stringVal;
			if (index != numberOfEntries - 1)
			{
				dataStructAsString += ", ";
			}
		}
		dataStructAsString += "]\n";
	}

	const ot::GenericDataStructMatrix* matrix = dynamic_cast<const ot::GenericDataStructMatrix*>(_dataStruct);
	if (matrix != nullptr)
	{
		uint32_t rows = matrix->getNumberOfRows();
		uint32_t columns = matrix->getNumberOfColumns();

		ot::MatrixEntryPointer matrixEntry;
		for (matrixEntry.m_row = 0; matrixEntry.m_row < rows; matrixEntry.m_row++)
		{
			dataStructAsString += "[";
			for (matrixEntry.m_column = 0; matrixEntry.m_column < columns; matrixEntry.m_column++)
			{
				const ot::Variable& var = matrix->getValue(matrixEntry);
				const std::string stringVal = converter(var);
				dataStructAsString += stringVal;
				if (matrixEntry.m_column != columns - 1)
				{
					dataStructAsString += ", ";
				}
			}
			dataStructAsString += "]\n";
		}
	}
	return dataStructAsString;
}

std::string ot::toString(const PipelineDataDocument& _document)
{
	ot::VariableToStringConverter converter;
	std::string document("{\n");
	for (auto& parameter : _document.m_parameter)
	{
		document += parameter.first + ": " + converter(parameter.second) + "\n";
	}
	if (_document.m_quantity != nullptr)
	{
		document += "Quantity: " + toString(_document.m_quantity.get()) + "\n";
	}

	document += "}";

	return document;
}


