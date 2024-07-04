#include "SParameterMatrixHelper.h"

SParameterMatrixHelper::SParameterMatrixHelper(uint32_t portNumber)
	: m_portNumber(portNumber)
{
	m_values.reserve(portNumber);
	for (uint32_t i = 0; i < portNumber; i++)
	{
		std::vector<ot::Variable> zeroInitialised;
		zeroInitialised.reserve(portNumber);
		for (uint32_t i = 0; i < portNumber; i++)
		{
			zeroInitialised.push_back(0.0);
		}
		m_values.push_back(std::move(zeroInitialised));
	}
}

void SParameterMatrixHelper::setValue(uint32_t row, uint32_t column, ot::Variable&& value)
{
	m_values[row][column] = std::move(value);
}

void SParameterMatrixHelper::setValue(uint32_t row, uint32_t column, const ot::Variable& value)
{
	m_values[row][column] = value;
}

const ot::Variable& SParameterMatrixHelper::getValue(uint32_t row, uint32_t column) const
{
	return m_values[row][column];
}
