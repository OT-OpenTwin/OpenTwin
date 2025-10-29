// @otlicense

#pragma once
#include "SystemDependencies/SystemDependentDefines.h"
#include "Grid/GridIteratorComponent.h"

#include <cassert>
class IGridIteratorVolume
{
protected:
	index_t _volumeSize;
	index_t _currentIndex = 0;
	index_t _startIndex = 0;

	GridIteratorComponent * _iteratorXAxis = nullptr;
	GridIteratorComponent * _iteratorYAxis = nullptr;
	GridIteratorComponent * _iteratorZAxis = nullptr;

public:
	virtual ~IGridIteratorVolume()
	{
		delete _iteratorXAxis;
		delete _iteratorYAxis;
		delete _iteratorZAxis;
	}

	GridIteratorComponent * GetXAxisIterator() const;
	GridIteratorComponent * GetYAxisIterator() const;
	GridIteratorComponent * GetZAxisIterator() const;

	void Reset()
	{
		index_t temp;
		_iteratorXAxis->SetFromLastToFirst(temp);
		_iteratorYAxis->SetFromLastToFirst(temp);
		_iteratorZAxis->SetFromLastToFirst(temp);
		_currentIndex = _startIndex;
	}
	const index_t GetVolumenSize() const { return _volumeSize; };
	index_t GetCurrentIndex() const { return _currentIndex; };

	bool HasNext() const 
	{ 
		return	_iteratorZAxis->HasNext() ||
				_iteratorYAxis->HasNext() ||
				_iteratorXAxis->HasNext();
	};

	const index_t & GetNextIndex();
};
