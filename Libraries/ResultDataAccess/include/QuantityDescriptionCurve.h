// @otlicense
// File: QuantityDescriptionCurve.h
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

class __declspec(dllexport) QuantityDescriptionCurve : public QuantityDescription
{
public:
	QuantityDescriptionCurve()
	{
		m_metadataQuantity.dataDimensions.push_back(1);
	}

	void setDataPoints(std::list<ot::Variable> _dataPoints)
	{
		m_dataPoints = std::vector<ot::Variable>{ _dataPoints.begin(), _dataPoints.end() };
	}

	void reserveDatapointSize(uint64_t _size)
	{
		m_dataPoints.reserve(_size);
	}

	void addDatapoint(ot::Variable& _variable)
	{
		m_dataPoints.push_back(_variable);
	}

	void addDatapoint(ot::Variable&& _variable)
	{
		m_dataPoints.push_back(std::move(_variable));
	}
	const std::vector<ot::Variable>& getDataPoints() const
	{
		return m_dataPoints;
	}

private:
	std::vector<ot::Variable> m_dataPoints;
};
