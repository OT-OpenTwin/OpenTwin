#pragma once
#include "QuantityDescription.h"

class __declspec(dllexport) QuantityDescriptionSParameter :public QuantityDescription
{
public:
	QuantityDescriptionSParameter(uint64_t _reserveNumberOfEntries = 0)
	{
		reserve(_reserveNumberOfEntries);
	}

	QuantityDescriptionSParameter(uint32_t _portNumber, uint64_t _reserveNumberOfEntries = 0)
		:QuantityDescriptionSParameter(_reserveNumberOfEntries)
	{
		m_metadataQuantity.dataDimensions = { _portNumber, _portNumber };
	}

	QuantityDescriptionSParameter(QuantityDescriptionSParameter&& _other) noexcept
		: QuantityDescription(_other), m_quantityValuesFirst(std::move(_other.m_quantityValuesFirst)),m_quantityValuesSecond(std::move(_other.m_quantityValuesSecond))
	{};

	QuantityDescriptionSParameter& operator=(QuantityDescriptionSParameter&& _other) noexcept
	{
		QuantityDescription::operator=(std::move(_other));
		m_quantityValuesFirst =std::move(_other.m_quantityValuesFirst);
		m_quantityValuesSecond =std::move(_other.m_quantityValuesSecond);
		return *this;
	};
	QuantityDescriptionSParameter(const QuantityDescriptionSParameter& _other) = default;
	QuantityDescriptionSParameter& operator=(const QuantityDescriptionSParameter& _other) = default;

	void setNumberOfPorts(uint32_t _portNumber)
	{
		m_metadataQuantity.dataDimensions = { _portNumber, _portNumber };
	}

	//! @brief The value matrices have to be initiated. Currently only dense matrices are handled an so the s-parameter matrices are initially padded with zeros.
	//! @param _numberOfValues 
	void initiateZeroFilledValueMatrices(uint64_t _numberOfValues)
	{
		m_quantityValuesFirst.reserve(_numberOfValues);
		for (uint64_t i = 0; i < _numberOfValues; i++)
		{
			m_quantityValuesFirst.push_back(SParameterMatrixHelper(m_metadataQuantity.dataDimensions[0]));
		}

		m_quantityValuesSecond.reserve(_numberOfValues);
		for (uint64_t i = 0; i < _numberOfValues; i++)
		{
			m_quantityValuesSecond.push_back(SParameterMatrixHelper(m_metadataQuantity.dataDimensions[0]));
		}
	}

	void reserve(uint64_t _reserveNumberOfEntries)
	{
		if (_reserveNumberOfEntries != 0)
		{
			m_quantityValuesFirst.reserve(_reserveNumberOfEntries);
			m_quantityValuesSecond.reserve(_reserveNumberOfEntries);
		}
	}

	void optimiseMemory()
	{
		m_quantityValuesFirst.shrink_to_fit();
		m_quantityValuesSecond.shrink_to_fit();
	}

	void pushBackFirstValue(SParameterMatrixHelper&& _sparameterMatrix)
	{
		m_quantityValuesFirst.push_back(_sparameterMatrix);
	}
	
	void pushBackSecondValue(SParameterMatrixHelper&& _sparameterMatrix)
	{
		m_quantityValuesSecond.push_back(_sparameterMatrix);
	}

	//! @brief Depending on the chosen value format, the first value either a real value, the magnitude or the decible value
	void setFirstValue(uint64_t _index, uint32_t _row, uint32_t _column, ot::Variable&& _value)
	{
		PRE(firstValueAccessValid(_index, _row, _column));
		m_quantityValuesFirst[_index].setValue(_row, _column, std::move(_value));
	}

	//! @brief Depending on the chosen value format, the second value either an imaginary value or the phase
	void setSecondValue(uint64_t _index, uint32_t _row, uint32_t _column, ot::Variable&& _value)
	{
		PRE(secondValueAccessValid(_index, _row, _column));
		m_quantityValuesSecond[_index].setValue(_row, _column, std::move(_value));
	}

	//! @brief Depending on the chosen value format, the first value either a real value, the magnitude or the decible value
	const ot::Variable& getFirstValue(uint64_t _index, uint32_t _row, uint32_t _column)
	{
		PRE(firstValueAccessValid(_index, _row, _column));
		return m_quantityValuesFirst[_index].getValue(_row, _column);
	}

	//! @brief Depending on the chosen value format, the second value either an imaginary value or the phase
	const ot::Variable& getSecondValue(uint64_t _index, uint32_t _row, uint32_t _column)
	{
		PRE(secondValueAccessValid(_index, _row, _column));
		return m_quantityValuesSecond[_index].getValue(_row, _column);
	}

	//! @brief Depending on the chosen value format, the first value either a real value, the magnitude or the decible value
	const size_t getNumberOfFirstValues() const { return m_quantityValuesFirst.size(); }

	//! @brief Depending on the chosen value format, the second value either an imaginary value or the phase
	const size_t getNumberOfSecondValues() const { return m_quantityValuesSecond.size(); }
private:
	std::vector<SParameterMatrixHelper> m_quantityValuesFirst;
	std::vector<SParameterMatrixHelper> m_quantityValuesSecond;

	bool inline firstValueAccessValid(uint64_t _index, uint32_t _row, uint32_t _column)
	{
		return m_quantityValuesFirst.size() > _index && m_quantityValuesFirst[0].getPortNumber() > _column && m_quantityValuesFirst[0].getPortNumber() > _row;
	}

	bool inline secondValueAccessValid(uint64_t _index, uint32_t _row, uint32_t _column)
	{
		return m_quantityValuesSecond.size() > _index && m_quantityValuesSecond[0].getPortNumber() > _column && m_quantityValuesSecond[0].getPortNumber() > _row;
	}

};
