#pragma once
#include "Grid/GridIteratorComponent.h"


class GridIteratorComponentSubspace : public GridIteratorComponent
{
public:
	GridIteratorComponentSubspace(index_t startPoint, index_t endPoint, index_t stepWidth)
		: GridIteratorComponent(startPoint, endPoint, stepWidth) {};

	bool SetSubspace(index_t startPoint, index_t endPoint)
	{
		if (endPoint >= startPoint && endPoint <= _endPoint && startPoint >= _startPoint)
		{
			_startPoint = startPoint;
			_currentPoint = startPoint;
			_endPoint = endPoint;
			SetResetDistance();
			return true;
		};
		return false;
	}
};