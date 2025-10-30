// @otlicense
// File: EdgeDiscretization.h
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
#include "SystemDependencies/SystemDependentDefines.h"
#include "MeshDiscretization.h"

#include <stdexcept>

class EdgeDiscretization : public MeshDiscretization
{

private:
	const double* deltaX = nullptr;
	const double* deltaY = nullptr;
	const double* deltaZ = nullptr;
 
public:
	explicit EdgeDiscretization(){};
	~EdgeDiscretization()
	{
		deltaX = nullptr;
		deltaY = nullptr;
		deltaZ = nullptr;
	};
	EdgeDiscretization(EdgeDiscretization & other) = delete;
	EdgeDiscretization & operator=(const EdgeDiscretization & other) = delete;

	void ExtractComponentsFromSingleVector(const std::vector<double> & surfaceDiscretizations) override;
	
	inline const double GetDeltaXAtIndex(index_t index) const;
	inline const double GetDeltaYAtIndex(index_t index) const;
	inline const double GetDeltaZAtIndex(index_t index) const;
};

inline const double EdgeDiscretization::GetDeltaXAtIndex(index_t index) const
{
	if (index < numberOfNodes)
	{
		return deltaX[index];
	}
	else
	{
		throw std::out_of_range(outOfRangeMessage);
	}
}

inline const double EdgeDiscretization::GetDeltaYAtIndex(index_t index) const
{
	if (index < numberOfNodes)
	{
		return deltaY[index];
	}
	else
	{
		throw std::out_of_range(outOfRangeMessage);
	}
}

inline const double EdgeDiscretization::GetDeltaZAtIndex(index_t index) const
{
	if (index < numberOfNodes)
	{
		return deltaZ[index];
	}
	else
	{
		throw std::out_of_range(outOfRangeMessage);
	}
}