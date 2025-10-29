// @otlicense

#pragma once
#include "SystemDependencies/SystemDependentDefines.h"
#include "Grid/IGridIteratorVolume.h"
#include "Grid/GridIteratorComponentSubspace.h"
#include "Grid/GridIteratorVolume.h"

class GridIteratorSubvolume : public IGridIteratorVolume
{
	void CalculateSubvolumeSize();

public:
	GridIteratorSubvolume(GridIteratorVolume & volumeIterator);
	
	bool SetXSubspace(index_t startPoint, index_t endPoint);
	bool SetYSubspace(index_t startPoint, index_t endPoint);
	bool SetZSubspace(index_t startPoint, index_t endPoint);

	GridIteratorVolume * CreateIteratorVolumeFromSubvolume();
};