#pragma once

#include "Grid/DimensionPadding.h"
#include "SystemDependencies/SystemProperties.h"

template<class T>
DimensionPadding<T>::DimensionPadding(Alignment alignment, index_t degreesOfFreedom)
{
	auto properties = SystemProperties::GetInstance();
	int bitsPerByte = 8;
	if (alignment == AVX512 && properties.CPUSupportsAVX512())
	{
		_alignment = AVX512;
		// Sizeof gives number of bytes, alignments are given in bit
		_lengthOfIntrinsicVector = static_cast<int>(AVX512) / (sizeof(T) * bitsPerByte);
		_degreesOfFreedomPadded = static_cast<index_t>(_lengthOfIntrinsicVector) * static_cast<index_t>(ceilf(degreesOfFreedom / static_cast<float>(_lengthOfIntrinsicVector)));
		_numberOfIntrinsicVectors = _degreesOfFreedomPadded / _lengthOfIntrinsicVector;

		_numberOfPaddedEntries = static_cast<int>(_degreesOfFreedomPadded - degreesOfFreedom);
	}
	else if ((alignment == AVX2|| alignment == AVX512) && properties.CPUSupportsAVX2())
	{
		_alignment = AVX2;
		// Sizeof gives number of bytes, alignments are given in bit
		_lengthOfIntrinsicVector = static_cast<int>(AVX2) / (sizeof(T) * bitsPerByte);
		_degreesOfFreedomPadded = static_cast<index_t>(_lengthOfIntrinsicVector) * static_cast<index_t>(ceilf(degreesOfFreedom / static_cast<float>(_lengthOfIntrinsicVector)));
		_numberOfIntrinsicVectors = _degreesOfFreedomPadded / _lengthOfIntrinsicVector;

		_numberOfPaddedEntries = static_cast<int>(_degreesOfFreedomPadded - degreesOfFreedom);
	}
	else //AVX or lower currently not supported (not planned to be)
	{
		_lengthOfIntrinsicVector = 1;
		_degreesOfFreedomPadded = degreesOfFreedom;
		_alignment = CacheLine64;
		return;
	}
	_masks.reset(new IntrinsicMasks(_numberOfIntrinsicVectors, _lengthOfIntrinsicVector, _numberOfPaddedEntries));
}
