// @otlicense
// File: IntrinsicMasks.cpp
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

#include "Grid/IntrinsicMasks.h"
#include <exception>

IntrinsicMasks::IntrinsicMasks(index_t numberOfIntrinsicVectors, int lengthOfIntrinsicVector, int numberOfPaddedEntries)
	: _numberOfIntrinsicVectors(numberOfIntrinsicVectors), _lengthOfIntrinsicVector(lengthOfIntrinsicVector), _numberOfPaddedEntries(numberOfPaddedEntries)
{
	//First dof is always boundary and requires treatment outside of the regular loop.
	_maskBeginNoBeginningExclude |= 1i16 << 0;
	int partialFilling = _lengthOfIntrinsicVector - _numberOfPaddedEntries;

	if (_numberOfIntrinsicVectors == 1)
	{
		for (int i = 1; i < partialFilling - 1; i++)
		{
			_maskBegin |= 1i16 << i;
			_maskBeginNoBeginningExclude |= 1i16 << i;
		}
	}
	else
	{
		for (int i = 1; i < _lengthOfIntrinsicVector; i++)
		{
			_maskBegin |= 1i16 << i;
			_maskBeginNoBeginningExclude |= 1i16 << i;
		}

		if (_numberOfPaddedEntries != 0)
		{

			for (int i = 0; i < partialFilling - 1; i++)
			{
				_maskEnd |= 1i16 << i;
			}
		}
		else
		{
			for (int i = 0; i < _lengthOfIntrinsicVector - 1; i++)
			{
				_maskEnd |= 1i16 << i;
			}
		}
	}
}

int IntrinsicMasks::GetMaskBeginNoBeginningExcludeBit(int bitIndex) const
{
	if (bitIndex > _lengthOfIntrinsicVector - 1)
	{
		throw std::exception("Intrinsic Mask length doesn't include the requested bit.");
	}
	unsigned short bitExtractingMask = 1i16 << bitIndex;
	bitExtractingMask &= _maskBeginNoBeginningExclude;
	int thebit = bitExtractingMask >> bitIndex;
	return thebit;
}

int IntrinsicMasks::GetMaskBeginBit(int bitIndex) const 
{
	if (bitIndex > _lengthOfIntrinsicVector - 1)
	{
		throw  std::exception("Intrinsic Mask length doesn't include the requested bit.");
	}
	unsigned short bitExtractingMask = 1i16 << bitIndex;
	bitExtractingMask &= _maskBegin;
	int thebit = bitExtractingMask >> bitIndex;
	return thebit;
}

int IntrinsicMasks::GetMaskEndBit(int bitIndex) const 
{
	if (bitIndex > _lengthOfIntrinsicVector - 1)
	{
		throw  std::exception("Intrinsic Mask length doesn't include the requested bit.");
	}
	unsigned short bitExtractingMask = 1i16 << bitIndex;
	bitExtractingMask &= _maskEnd;
	int thebit = bitExtractingMask >> bitIndex;
	return thebit;
}

