#pragma once
#include "QuantityDescription.h"
#include "OTCore/GenericDataStructMatrix.h"

class __declspec(dllexport) QuantityDescriptionMatrix : public QuantityDescription
{
public:
	void setValues(const std::vector<ot::GenericDataStructMatrix>& _values) { m_quantityValues = _values; };
	void addToValues(const ot::GenericDataStructMatrix& _value) { m_quantityValues.push_back(_value); }
	const std::vector<ot::GenericDataStructMatrix>& getValues() const { return m_quantityValues; }
private:
	std::vector<ot::GenericDataStructMatrix> m_quantityValues;
};
