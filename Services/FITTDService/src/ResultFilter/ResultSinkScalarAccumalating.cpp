#include "ResultFilter/ResultSinkScalarAccumalating.h"

void ResultSinkScalarAccumalating::ConsumeResultContainer(ResultContainer * container)
{
	auto newContainer = dynamic_cast<ResultContainerScalar*>(container);
	assert(newContainer != nullptr);
	
	_timeSteps.push_back(newContainer->GetCurrentTimestep()*_deltaT);
	int i = 0;
	_volumeIterator->Reset();
	_result.push_back(newContainer->GetResult()[i]);
	while (_volumeIterator->HasNext())
	{
		i++;
		_volumeIterator->GetNextIndex();
		_result.push_back(newContainer->GetResult()[i]);
	}
	delete newContainer;
	newContainer = nullptr;
}

void ResultSinkScalarAccumalating::PerformPostProcessing()
{
}

void ResultSinkScalarAccumalating::SetVolumeIterator(IGridIteratorVolume * volumeIter)
{
	ResultSink::SetVolumeIterator(volumeIter);
	_timeSteps.reserve(_nbOfExecutions);
	_result.reserve(_nbOfExecutions * _volumeIterator->GetVolumenSize());
}