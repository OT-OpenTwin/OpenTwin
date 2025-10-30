// @otlicense
// File: QuantityDescriptionMatrix.h
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
