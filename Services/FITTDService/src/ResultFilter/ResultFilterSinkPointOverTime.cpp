#include "ResultFilter/ResultFilterSinkPointOverTime.h"


ResultFilterSinkPointOverTime::ResultFilterSinkPointOverTime(int executionFrequency, int totalTimeSteps, std::vector<ResultFilter*> finalFilter, double deltaT)
: ResultFilterSink(executionFrequency, totalTimeSteps, finalFilter), _deltaT(deltaT)
{
	_resultCategorie = scalar;
	_timeSteps.reserve(GetNumberOfExecutions());
}

void ResultFilterSinkPointOverTime::SetVolumeIterator(IGridIteratorVolume * volumenFilter)
{
	ResultFilterSink::SetVolumeIterator(volumenFilter);
	assert(_volumenIterator->GetVolumenSize() == 1);
	int collectionSize = GetNumberOfExecutions() * _volumenIterator->GetVolumenSize();
	_resultCollection.reserve(collectionSize);
}


void ResultFilterSinkPointOverTime::ExecuteFilter(int currentTimestep, double * input)
{
	_timeSteps.push_back(currentTimestep * _deltaT);
	for (int i = 0; i < _volumenIterator->GetVolumenSize(); i++)
	{
		_resultCollection.push_back(input[i]);
	}
	for (auto filter : _finalFilter)
	{
		filter->ExecuteIfFrequencyMatches(currentTimestep, _resultCollection.data());
	}
}