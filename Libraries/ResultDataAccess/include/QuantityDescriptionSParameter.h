#pragma once
#include "QuantityDescription.h"
#include "OTCore/GenericDataStructMatrix.h"
class __declspec(dllexport) QuantityDescriptionSParameter :public QuantityDescription
{
public:
	QuantityDescriptionSParameter(uint64_t _reserveNumberOfEntries = 0);

	QuantityDescriptionSParameter(uint32_t _portNumber, uint64_t _reserveNumberOfEntries = 0);

	QuantityDescriptionSParameter(QuantityDescriptionSParameter&& _other) noexcept = default;
	QuantityDescriptionSParameter& operator=(QuantityDescriptionSParameter&& _other) noexcept = default;
	QuantityDescriptionSParameter(const QuantityDescriptionSParameter& _other) = default;
	QuantityDescriptionSParameter& operator=(const QuantityDescriptionSParameter& _other) = default;

	void setNumberOfPorts(uint32_t _portNumber);

	//! @brief The value matrices have to be initiated. Currently only dense matrices are handled an so the s-parameter matrices are initially padded with zeros.
	//! @param _numberOfValues 
	void initiateZeroFilledValueMatrices(uint64_t _numberOfValues);

	void reserve(uint64_t _reserveNumberOfEntries);

	void optimiseMemory();

	void pushBackFirstValue(ot::GenericDataStructMatrix&& _sparameterMatrix);
	
	void pushBackSecondValue(ot::GenericDataStructMatrix&& _sparameterMatrix);

	//! @brief Depending on the chosen value format, the first value either a real value, the magnitude or the decible value
	void setFirstValue(uint64_t _index, const ot::MatrixEntryPointer& _matrixPointer, ot::Variable&& _value);

	//! @brief Depending on the chosen value format, the second value either an imaginary value or the phase
	void setSecondValue(uint64_t _index, const ot::MatrixEntryPointer& _matrixPointer, ot::Variable&& _value);

	//! @brief Depending on the chosen value format, the first value either a real value, the magnitude or the decible value
	//const ot::Variable& getFirstValue(uint64_t _index, uint32_t _row, uint32_t _column);
	const std::vector<ot::Variable> getFirstValues(uint64_t _index);

	//! @brief Depending on the chosen value format, the second value either an imaginary value or the phase
	//const ot::Variable& getSecondValue(uint64_t _index, uint32_t _row, uint32_t _column);
	const std::vector<ot::Variable> getSecondValues(uint64_t _index);

	//! @brief Depending on the chosen value format, the first value either a real value, the magnitude or the decible value
	const size_t getNumberOfFirstValues() const { return m_quantityValuesFirst.size(); }

	//! @brief Depending on the chosen value format, the second value either an imaginary value or the phase
	const size_t getNumberOfSecondValues() const { return m_quantityValuesSecond.size(); }
private:
	uint64_t m_numberOfMatrixEntries = 0;
	std::vector<ot::GenericDataStructMatrix> m_quantityValuesFirst;
	std::vector<ot::GenericDataStructMatrix> m_quantityValuesSecond;

	bool inline firstValueAccessValid(uint64_t _index, uint32_t _row, uint32_t _column);

	bool inline secondValueAccessValid(uint64_t _index, uint32_t _row, uint32_t _column);

};
