#pragma once
#include "QuantityDescription.h"
#include "OTCore/GenericDataStructMatrix.h"

class __declspec(dllexport) QuantityDescriptionMatrix : public QuantityDescription
{
public:
	QuantityDescriptionMatrix(const ot::MatrixEntryPointer& _matrixDimensions)
	{
		m_metadataQuantity.dataDimensions = { _matrixDimensions.m_row, _matrixDimensions.m_column };
	}
	QuantityDescriptionMatrix(const ot::MatrixEntryPointer& _matrixDimensions, uint64_t _reserveNumberOfEntries)
		:QuantityDescriptionMatrix(_matrixDimensions)
	{
		m_quantityValues.reserve(_reserveNumberOfEntries);
	}
	void setValues(const std::vector<ot::GenericDataStructMatrix>& _values) { m_quantityValues = _values; };
	void addToValues(const ot::GenericDataStructMatrix& _value) { m_quantityValues.push_back(_value); }
	const std::vector<ot::GenericDataStructMatrix>& getValues() const { return m_quantityValues; }
private:
	std::vector<ot::GenericDataStructMatrix> m_quantityValues;
};
