// @otlicense
// File: Datapoints.h
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
#include <vector>

struct Datapoints
{
	std::vector<double> m_xData;
	std::vector<double> m_yData;
	void reserve(size_t numberOfDataPoints)
	{
		m_xData.reserve(numberOfDataPoints);
		m_yData.reserve(numberOfDataPoints);
	}
	void shrinkToFit()
	{
		m_xData.shrink_to_fit();
		m_yData.shrink_to_fit();
	}
};
