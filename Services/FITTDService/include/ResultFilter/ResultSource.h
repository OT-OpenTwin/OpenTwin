#pragma once
#include "ResultFilter/ResultFilter.h"
#include "Grid/GridIteratorVolume.h"
#include "Grid/IGridIteratorVolume.h"
#include "ResultFilter/ResultContainer.h"
#include "ResultHandling/ExecutionBarrier.h"

class ResultSource
{
public:
	~ResultSource();
	ResultSource(ExecutionBarrier * executionBarrier , IGridIteratorVolume * volumeIter);
	IGridIteratorVolume * GetSubvolumeIterator() const { return _subVolumeIterator; };
	int GetCurrentTimestep() const { return _currentTimestep; };

	bool ShallBeExecuted(int currentTimestep)
	{
		return _executionBarrier->AllowExecution(currentTimestep);
	}

	virtual ResultContainer * GenerateResultContainer(int currentTimeStep) = 0;

protected:
	IGridIteratorVolume * _volumeIterator = nullptr;
	IGridIteratorVolume * _subVolumeIterator = nullptr;

	int _currentTimestep;
private:
	ExecutionBarrier * _executionBarrier = nullptr;

};