#pragma once
#include "QuantityDescription.h"

class __declspec(dllexport) QuantityDescriptionCurveComplex :public QuantityDescription
{
public:
	QuantityDescriptionCurveComplex()
	{
		m_metadataQuantity.dataDimensions.push_back(1);
	}

	void reserveSizeRealValues(uint64_t _size)
	{
		m_quantityValuesReal.reserve(_size);
	}
	void reserveSizeImagValues(uint64_t _size)
	{
		m_quantityValuesImag.reserve(_size);
	}
	void addValueReal(ot::Variable&& _value)
	{
		m_quantityValuesReal.push_back(_value);
	}
	void addValueImag(ot::Variable&& _value)
	{
		m_quantityValuesImag.push_back(_value);
	}
	const std::vector<ot::Variable>& getQuantityValuesReal() const
	{
		return m_quantityValuesReal;
	}
	const std::vector<ot::Variable>& getQuantityValuesImag() const
	{
		return m_quantityValuesImag;
	}

private:
	std::vector<ot::Variable> m_quantityValuesReal;
	std::vector<ot::Variable> m_quantityValuesImag;
};