// @otlicense

#include "ResultFilter/ResultSource.h"
#include "Grid/GridIteratorSubvolume.h"

ResultSource::~ResultSource()
{
	if (dynamic_cast<GridIteratorSubvolume*>(_subVolumeIterator) != nullptr)
	{
		delete _subVolumeIterator;
	}
	delete _executionBarrier;
	_executionBarrier = nullptr;
}

ResultSource::ResultSource(ExecutionBarrier * executionBarrier, IGridIteratorVolume * volumeIter)
	:_executionBarrier(executionBarrier)
{
	_volumeIterator = volumeIter;
	auto temp = dynamic_cast<GridIteratorSubvolume*>(_volumeIterator);
	if (temp != nullptr)
	{
		_subVolumeIterator = temp->CreateIteratorVolumeFromSubvolume();
	}
	else
	{
		_subVolumeIterator = _volumeIterator;
	}
}
