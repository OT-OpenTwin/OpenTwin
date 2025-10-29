// @otlicense

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