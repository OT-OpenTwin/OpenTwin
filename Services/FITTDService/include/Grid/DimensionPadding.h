// @otlicense
// File: DimensionPadding.h
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

#include <memory>

#include "SystemDependencies/SystemDependentDefines.h"
#include "SystemDependencies/Allignments.h"
#include "Grid/IntrinsicMasks.h"

template<class T>
class DimensionPadding
{
public:
	DimensionPadding(Alignment alignment, index_t degreesOfFreedom);
	DimensionPadding(DimensionPadding & other) = delete;
	DimensionPadding & operator=(DimensionPadding & other) = delete;

	const index_t GetPaddedDegreesOfFreedom() const { return _degreesOfFreedomPadded; };
	const index_t GetNumberOfIntrinsicVectors() const { return _numberOfIntrinsicVectors; };

	const IntrinsicMasks * GetIntrinsicMasks() const { return _masks.get(); }

	const int GetLengthOfIntrinsicVector() const { return _lengthOfIntrinsicVector; };
	const Alignment GetAlignment() const { return _alignment; };
private:
	Alignment _alignment;

	index_t _degreesOfFreedomPadded = 0;
	int _numberOfPaddedEntries = 0;
	index_t _numberOfIntrinsicVectors = 0;

	int _lengthOfIntrinsicVector = 0;
	std::unique_ptr<IntrinsicMasks> _masks;

};

#include "Grid/DimensionPadding.hpp"