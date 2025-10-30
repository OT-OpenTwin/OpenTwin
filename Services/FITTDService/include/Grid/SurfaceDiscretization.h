// @otlicense
// File: SurfaceDiscretization.h
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
#include <string>
#include <stdexcept>

#include "SystemDependencies/SystemDependentDefines.h"
#include "MeshDiscretization.h"

class SurfaceDiscretization : public MeshDiscretization
{

private:
	const double* deltaXY =nullptr;
	const double* deltaXZ =nullptr;
	const double* deltaYZ =nullptr;
	
public:
	explicit SurfaceDiscretization(){};
	~SurfaceDiscretization() 
	{
		deltaXY = nullptr;
		deltaXZ = nullptr;
		deltaYZ = nullptr;
	};
	SurfaceDiscretization(SurfaceDiscretization & other) = delete;
	SurfaceDiscretization & operator=(const SurfaceDiscretization & other) = delete;

	void ExtractComponentsFromSingleVector(const std::vector<double> & surfaceDiscretizations) override;

	inline const double GetDeltaXYAtIndex(index_t index) const;
	inline const double GetDeltaXZAtIndex(index_t index) const;
	inline const double GetDeltaYZAtIndex(index_t index) const;
	
};

inline const double SurfaceDiscretization::GetDeltaXYAtIndex(index_t index) const
{
	if (index < numberOfNodes)
	{
		return deltaXY[index];
	}
	else
	{
		throw std::out_of_range(outOfRangeMessage);
	}
}

inline const double SurfaceDiscretization::GetDeltaXZAtIndex(index_t index) const
{
	if (index < numberOfNodes)
	{
		return deltaXZ[index];
	}
	else
	{
		throw std::out_of_range(outOfRangeMessage);
	}
}

inline const double SurfaceDiscretization::GetDeltaYZAtIndex(index_t index) const
{
	if (index < numberOfNodes)
	{
		return deltaYZ[index];
	}
	else
	{
		throw std::out_of_range(outOfRangeMessage);
	}
}