#include "QuantityDescriptionSParameter.h"

QuantityDescriptionSParameter::QuantityDescriptionSParameter(uint64_t _reserveNumberOfEntries)
{
	reserve(_reserveNumberOfEntries);
}

QuantityDescriptionSParameter::QuantityDescriptionSParameter(uint32_t _portNumber, uint64_t _reserveNumberOfEntries)
	:QuantityDescriptionSParameter(_reserveNumberOfEntries)
{
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
	for (uint64_t i = 0; i < _numberOfValues; i++)
	{
		m_quantityValuesFirst.push_back(ot::GenericDataStructMatrix(numberOfPorts, numberOfPorts));
	}

	m_quantityValuesSecond.reserve(_numberOfValues);
	for (uint64_t i = 0; i < _numberOfValues; i++)
	{
		m_quantityValuesSecond.push_back(ot::GenericDataStructMatrix(numberOfPorts, numberOfPorts));
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

void QuantityDescriptionSParameter::setFirstValue(uint64_t _index, uint32_t _row, uint32_t _column, ot::Variable&& _value)
{
	PRE(firstValueAccessValid(_index, _row, _column));
	m_quantityValuesFirst[_index].setValue(_row, _column, std::move(_value));
}

void QuantityDescriptionSParameter::setSecondValue(uint64_t _index, uint32_t _row, uint32_t _column, ot::Variable&& _value)
{
	PRE(secondValueAccessValid(_index, _row, _column));
	m_quantityValuesSecond[_index].setValue(_row, _column, std::move(_value));
}

const ot::Variable& QuantityDescriptionSParameter::getFirstValue(uint64_t _index, uint32_t _row, uint32_t _column)
{
	PRE(firstValueAccessValid(_index, _row, _column));
	return m_quantityValuesFirst[_index].getValue(_row, _column);
}

const ot::Variable& QuantityDescriptionSParameter::getSecondValue(uint64_t _index, uint32_t _row, uint32_t _column)
{
	PRE(secondValueAccessValid(_index, _row, _column));
	return m_quantityValuesSecond[_index].getValue(_row, _column);
}

inline bool QuantityDescriptionSParameter::firstValueAccessValid(uint64_t _index, uint32_t _row, uint32_t _column)
{
	return m_quantityValuesFirst.size() > _index && m_quantityValuesFirst[0].getNumberOfColumns() > _column && m_quantityValuesFirst[0].getNumberOfRows() > _row;
}

inline bool QuantityDescriptionSParameter::secondValueAccessValid(uint64_t _index, uint32_t _row, uint32_t _column)
{
	return m_quantityValuesSecond.size() > _index && m_quantityValuesSecond[0].getNumberOfColumns() > _column && m_quantityValuesSecond[0].getNumberOfRows() > _row;
}
