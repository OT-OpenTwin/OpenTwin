// @otlicense

#include "Grid/IGridIteratorVolume.h"

const index_t & IGridIteratorVolume::GetNextIndex()
{
	if (_iteratorXAxis->HasNext())
	{
		_iteratorXAxis->SetToNext(_currentIndex);
	}
	else
	{
		_iteratorXAxis->SetFromLastToFirst(_currentIndex);
		if (_iteratorYAxis->HasNext())
		{
			_iteratorYAxis->SetToNext(_currentIndex);
		}
		else
		{
			assert(_iteratorZAxis->HasNext());
			_iteratorYAxis->SetFromLastToFirst(_currentIndex);
			_iteratorZAxis->SetToNext(_currentIndex);
		}
	}
	return _currentIndex;
}

GridIteratorComponent * IGridIteratorVolume::GetXAxisIterator() const
{
	return _iteratorXAxis;
}

GridIteratorComponent * IGridIteratorVolume::GetYAxisIterator() const
{
	return _iteratorYAxis;
}

GridIteratorComponent * IGridIteratorVolume::GetZAxisIterator() const
{
	return _iteratorZAxis;
}
