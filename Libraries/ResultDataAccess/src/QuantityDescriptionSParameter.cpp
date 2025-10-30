// @otlicense
// File: QuantityDescriptionSParameter.cpp
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

#include "QuantityDescriptionSParameter.h"

QuantityDescriptionSParameter::QuantityDescriptionSParameter(uint64_t _reserveNumberOfEntries)
{
	reserve(_reserveNumberOfEntries);
}

QuantityDescriptionSParameter::QuantityDescriptionSParameter(uint32_t _portNumber, uint64_t _reserveNumberOfEntries)
	:QuantityDescriptionSParameter(_reserveNumberOfEntries)
{
	m_numberOfMatrixEntries = _portNumber * _portNumber;
	m_metadataQuantity.dataDimensions = { _portNumber, _portNumber };
}

void QuantityDescriptionSParameter::setNumberOfPorts(uint32_t _portNumber)
{
	m_metadataQuantity.dataDimensions = { _portNumber, _portNumber };
}

void QuantityDescriptionSParameter::initiateZeroFilledValueMatrices(uint64_t _numberOfValues)
{
	m_quantityValuesFirst.reserve(_numberOfValues);
	uint32_t numberOfPorts =	m_metadataQuantity.dataDimensions[0];
	ot::MatrixEntryPointer matrixPointer;
	matrixPointer.m_column = numberOfPorts;
	matrixPointer.m_row = numberOfPorts;
	for (uint64_t i = 0; i < _numberOfValues; i++)
	{
		m_quantityValuesFirst.push_back(ot::GenericDataStructMatrix(matrixPointer));
	}

	m_quantityValuesSecond.reserve(_numberOfValues);
	for (uint64_t i = 0; i < _numberOfValues; i++)
	{
		m_quantityValuesSecond.push_back(ot::GenericDataStructMatrix(matrixPointer));
	}
}

void QuantityDescriptionSParameter::reserve(uint64_t _reserveNumberOfEntries)
{
	if (_reserveNumberOfEntries != 0)
	{
		m_quantityValuesFirst.reserve(_reserveNumberOfEntries);
		m_quantityValuesSecond.reserve(_reserveNumberOfEntries);
	}
}

void QuantityDescriptionSParameter::optimiseMemory()
{
	m_quantityValuesFirst.shrink_to_fit();
	m_quantityValuesSecond.shrink_to_fit();
}

void QuantityDescriptionSParameter::pushBackFirstValue(ot::GenericDataStructMatrix&& _sparameterMatrix)
{
	m_quantityValuesFirst.push_back(_sparameterMatrix);
}

void QuantityDescriptionSParameter::pushBackSecondValue(ot::GenericDataStructMatrix&& _sparameterMatrix)
{
	m_quantityValuesSecond.push_back(_sparameterMatrix);
}

void QuantityDescriptionSParameter::setFirstValue(uint64_t _index, const ot::MatrixEntryPointer& _matrixPointer, ot::Variable&& _value)
{
	PRE(firstValueAccessValid(_index, _matrixPointer.m_row , _matrixPointer.m_column));
	m_quantityValuesFirst[_index].setValue(_matrixPointer, std::move(_value));
}

void QuantityDescriptionSParameter::setSecondValue(uint64_t _index, const ot::MatrixEntryPointer& _matrixPointer, ot::Variable&& _value)
{
	PRE(secondValueAccessValid(_index, _matrixPointer.m_row, _matrixPointer.m_column));
	m_quantityValuesSecond[_index].setValue(_matrixPointer, std::move(_value));
}

//const ot::Variable& QuantityDescriptionSParameter::getFirstValue(uint64_t _index, uint32_t _row, uint32_t _column)
//{
//	PRE(firstValueAccessValid(_index, _row, _column));
//	return m_quantityValuesFirst[_index].getValue(_row, _column);
//}

const std::vector<ot::Variable> QuantityDescriptionSParameter::getFirstValues(uint64_t _index)
{
	PRE(m_quantityValuesFirst.size() > _index);
	const ot::Variable* values =m_quantityValuesFirst[_index].getValues();
	return std::vector<ot::Variable>(&values[0], &values[m_numberOfMatrixEntries]);
}

//const ot::Variable& QuantityDescriptionSParameter::getSecondValue(uint64_t _index, uint32_t _row, uint32_t _column)
//{
//	PRE(secondValueAccessValid(_index, _row, _column));
//	return m_quantityValuesSecond[_index].getValue(_row, _column);
//}

const std::vector<ot::Variable> QuantityDescriptionSParameter::getSecondValues(uint64_t _index)
{
	PRE(m_quantityValuesSecond.size() > _index);
	const ot::Variable* values = m_quantityValuesSecond[_index].getValues();
	return std::vector<ot::Variable>(&values[0], &values[m_numberOfMatrixEntries]);
}

inline bool QuantityDescriptionSParameter::firstValueAccessValid(uint64_t _index, uint32_t _row, uint32_t _column)
{
	return m_quantityValuesFirst.size() > _index && m_quantityValuesFirst[0].getNumberOfColumns() > _column && m_quantityValuesFirst[0].getNumberOfRows() > _row;
}

inline bool QuantityDescriptionSParameter::secondValueAccessValid(uint64_t _index, uint32_t _row, uint32_t _column)
{
	return m_quantityValuesSecond.size() > _index && m_quantityValuesSecond[0].getNumberOfColumns() > _column && m_quantityValuesSecond[0].getNumberOfRows() > _row;
}
