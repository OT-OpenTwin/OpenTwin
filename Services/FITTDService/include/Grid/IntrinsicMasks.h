// @otlicense
// File: IntrinsicMasks.h
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
#include "SystemDependencies/Allignments.h"
#include "SystemDependencies/SystemDependentDefines.h"

class IntrinsicMasks
{
protected:
	unsigned short _maskBeginNoBeginningExclude = 0i16;
	unsigned short _maskBegin = 0i16;
	unsigned short _maskEnd = 0i16;

	index_t _numberOfIntrinsicVectors;
	int _lengthOfIntrinsicVector;
	int _numberOfPaddedEntries;

public:
	IntrinsicMasks(index_t numberOfIntrinsicVectors, int lengthOfIntrinsicVector, int numberOfPaddedEntries);

	int GetMaskBeginNoBeginningExcludeBit(int bitIndex) const;
	int GetMaskBeginBit(int bitIndex) const;
	int GetMaskEndBit(int bitIndex) const;
};