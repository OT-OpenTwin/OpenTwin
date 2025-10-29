// @otlicense

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