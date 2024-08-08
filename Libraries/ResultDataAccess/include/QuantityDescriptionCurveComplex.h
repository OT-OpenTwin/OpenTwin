#pragma once
#include "QuantityDescription.h"

class __declspec(dllexport) QuantityDescriptionCurveComplex :public QuantityDescription
{
public:
	
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
		PRE(m_metadataQuantity.valueDescriptions.size() >= 1);
		m_quantityValuesReal.push_back(_value);
	}
	void addValueImag(ot::Variable&& _value)
	{
		PRE(m_metadataQuantity.valueDescriptions.size() >= 2);
		m_quantityValuesImag.push_back(_value);
	}
	const std::vector<ot::Variable>& getQuantityValuesReal() const
	{
		PRE(m_metadataQuantity.valueDescriptions.size() >= 1);
		return m_quantityValuesReal;
	}
	const std::vector<ot::Variable>& getQuantityValuesImag() const
	{
		PRE(m_metadataQuantity.valueDescriptions.size() >= 2);
		return m_quantityValuesImag;
	}

private:
	std::vector<ot::Variable> m_quantityValuesReal;
	std::vector<ot::Variable> m_quantityValuesImag;
	MetadataQuantity m_metadataQuantity;
};