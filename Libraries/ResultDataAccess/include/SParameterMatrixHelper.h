#pragma once
#include <stdint.h>
#include <vector>

#include "OTCore/Variable.h"

class __declspec(dllexport) SParameterMatrixHelper
{
public:
	SParameterMatrixHelper(uint32_t portNumber);

	void setValue(uint32_t row, uint32_t column, ot::Variable&& value);
	void setValue(uint32_t row, uint32_t column, const ot::Variable& value);

	const ot::Variable& getValue(uint32_t row, uint32_t column) const;

	const uint32_t getPortNumber() const { return m_portNumber; }

private:
	uint32_t m_portNumber = 0;
	std::vector<std::vector<ot::Variable>> m_values;
};