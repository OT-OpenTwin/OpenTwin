// @otlicense
// File: QuantityDescriptionCurveComplex.h
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