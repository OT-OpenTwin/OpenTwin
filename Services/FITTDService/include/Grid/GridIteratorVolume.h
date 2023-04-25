#pragma once

#include "Grid/IGridIteratorVolume.h"
#include "Grid/GridIteratorComponent.h"

class GridIteratorVolume : public IGridIteratorVolume
{
public:
	GridIteratorVolume(GridIteratorComponent * iteratorXAxis, GridIteratorComponent * iteratorYAxis, GridIteratorComponent * iteratorZAxis, index_t volumeSize)
	{
		_iteratorXAxis = iteratorXAxis;
		_iteratorYAxis = iteratorYAxis;
		_iteratorZAxis = iteratorZAxis;
		_volumeSize = volumeSize;
	};
};