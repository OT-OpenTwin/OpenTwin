// @otlicense
// File: QuantityDescriptionSParameter.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
