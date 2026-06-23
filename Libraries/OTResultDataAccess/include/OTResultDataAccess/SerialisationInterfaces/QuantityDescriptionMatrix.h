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

// OpenTwin header
#include "OTCore/DataStruct/GenericDataStructMatrix.h"
#include "OTResultDataAccess/SerialisationInterfaces/QuantityDescription.h"

class OT_RESULTDATAACCESS_API_EXPORT QuantityDescriptionMatrix : public QuantityDescription
{
public:
	QuantityDescriptionMatrix(const ot::MatrixEntryPointer& _matrixDimensions);
	QuantityDescriptionMatrix(const ot::MatrixEntryPointer& _matrixDimensions, uint64_t _reserveNumberOfEntries);
	void setValues(const std::vector<ot::GenericDataStructMatrix>& _values);

	void initiateZeroFilledValueMatrices(uint64_t _numberOfEntries);
	void setValue(uint64_t _index, const ot::MatrixEntryPointer& _matrixEntry, ot::Variable& _value);

	void addToValues(const ot::GenericDataStructMatrix& _value) { m_quantityValues.push_back(_value); }
	
	const std::vector<ot::GenericDataStructMatrix>& getValues() const { return m_quantityValues; }
	uint64_t getNumberOfEntries() const;

	void optimizeMemory();

private:
	std::vector<ot::GenericDataStructMatrix> m_quantityValues;
};
