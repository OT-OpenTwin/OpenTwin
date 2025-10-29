// @otlicense

#pragma once
#include "Grid/IGridIteratorVolume.h"
#include <vector>


class ResultFilter
{
public:
	ResultFilter(int executionFrequency, int totalTimeSteps)
		: _executionFrequency(executionFrequency), _lastTimeStep(totalTimeSteps - 1) {};
	virtual ~ResultFilter() {};
	void ExecuteIfFrequencyMatches(int currentTimestep, double * input);
	bool FrequencyMatches(int currentTimestep);
	virtual void SetVolumeIterator(IGridIteratorVolume *  volumenFilter) = 0;
	const IGridIteratorVolume* GetVolumeIterator() { return _volumeIterator; };

protected:
	int _lastTimeStep = 0;
	IGridIteratorVolume *  _volumeIterator = nullptr;
	virtual void ExecuteFilter(int currentTimestep, double * input) = 0;

private:
	int _executionFrequency = 0;
};

