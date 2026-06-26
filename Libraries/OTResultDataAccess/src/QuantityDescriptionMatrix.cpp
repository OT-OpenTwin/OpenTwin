#include "OTResultDataAccess/SerialisationInterfaces/QuantityDescriptionMatrix.h"

void QuantityDescriptionMatrix::initiateZeroFilledValueMatrices(uint64_t _numberOfEntries)
{
	m_quantityValues.reserve(_numberOfEntries);
	
	ot::MatrixEntryPointer matrixPointer;
	matrixPointer.setColumn(m_metadataQuantity.dataDimensions[0]);
	matrixPointer.setRow(m_metadataQuantity.dataDimensions[1]);

	for (uint64_t i = 0; i < _numberOfEntries; i++)
	{
		m_quantityValues.push_back(ot::GenericDataStructMatrix(matrixPointer));
	}
}

QuantityDescriptionMatrix::QuantityDescriptionMatrix(const ot::MatrixEntryPointer& _matrixDimensions)
{
	m_metadataQuantity.dataDimensions = { _matrixDimensions.getRow(), _matrixDimensions.getColumn() };
}

QuantityDescriptionMatrix::QuantityDescriptionMatrix(const ot::MatrixEntryPointer& _matrixDimensions, uint64_t _reserveNumberOfEntries)
	:QuantityDescriptionMatrix(_matrixDimensions)
{
	m_quantityValues.reserve(_reserveNumberOfEntries);
}

void QuantityDescriptionMatrix::setValues(const std::vector<ot::GenericDataStructMatrix>& _values)
{ 
	m_quantityValues = _values; 
}

uint64_t QuantityDescriptionMatrix::getNumberOfEntries() const
{
	return m_quantityValues.size();
}

void QuantityDescriptionMatrix::optimizeMemory()
{
	m_quantityValues.shrink_to_fit();
}

void QuantityDescriptionMatrix::setValue(uint64_t _index, const ot::MatrixEntryPointer& _matrixEntry, ot::Variable& _value)
{
	m_quantityValues[_index].setValue(_matrixEntry, _value);
}

