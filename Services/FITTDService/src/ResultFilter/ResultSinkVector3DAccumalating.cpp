#include "ResultFilter/ResultSinkVector3DAccumalating.h"

void ResultSinkVector3DAccumalating::SetVolumeIterator(IGridIteratorVolume * volumeIter)
{
	ResultSink::SetVolumeIterator(volumeIter);
	_timeSteps.reserve(_nbOfExecutions);
	_resultX.reserve(_nbOfExecutions * _volumeIterator->GetVolumenSize());
	_resultY.reserve(_nbOfExecutions * _volumeIterator->GetVolumenSize());
	_resultZ.reserve(_nbOfExecutions * _volumeIterator->GetVolumenSize());
}

void ResultSinkVector3DAccumalating::ConsumeResultContainer(ResultContainer * container)
{
	auto newContainer = dynamic_cast<ResultContainerVector3DComplex*>(container);
	assert(newContainer != nullptr);

	_timeSteps.push_back(newContainer->GetCurrentTimestep()*_deltaT);
	int i = 0;
	_volumeIterator->Reset();
	_resultX.push_back(newContainer->GetRealComponentX()[i]);
	_resultY.push_back(newContainer->GetRealComponentY()[i]);
	_resultZ.push_back(newContainer->GetRealComponentZ()[i]);

	while (_volumeIterator->HasNext())
	{
		i++;
		_volumeIterator->GetNextIndex();
		_resultX.push_back(newContainer->GetRealComponentX()[i]);
		_resultY.push_back(newContainer->GetRealComponentY()[i]);
		_resultZ.push_back(newContainer->GetRealComponentZ()[i]);
	}
	delete newContainer;
	newContainer = nullptr;
}

void ResultSinkVector3DAccumalating::PerformPostProcessing()
{

}
