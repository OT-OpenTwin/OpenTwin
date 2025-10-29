// @otlicense

#include  "ResultFilter/ResultSinkVector3DComplexSum.h"

ResultSinkVector3DComplexSum::~ResultSinkVector3DComplexSum()
{
	if (_summedUpResult != nullptr)
	{
		delete _summedUpResult;
		_summedUpResult = nullptr;
	}
}

void ResultSinkVector3DComplexSum::ConsumeResultContainer(ResultContainer * container)
{
	ResultContainerVector3DComplex * newContainer = dynamic_cast<ResultContainerVector3DComplex*>(container);
	assert(newContainer != nullptr);
	if (_summedUpResult == nullptr)
	{
		_summedUpResult = newContainer;
	}
	else
	{
		int i = 0;
		_volumeIterator->Reset();
		while (_volumeIterator->HasNext())
		{
			_summedUpResult->GetRealComponentX()[i] += newContainer->GetRealComponentX()[i];
			_summedUpResult->GetRealComponentY()[i] += newContainer->GetRealComponentY()[i];
			_summedUpResult->GetRealComponentZ()[i] += newContainer->GetRealComponentZ()[i];
			
			_summedUpResult->GetImagComponentX()[i] += newContainer->GetImagComponentX()[i];
			_summedUpResult->GetImagComponentY()[i] += newContainer->GetImagComponentY()[i];
			_summedUpResult->GetImagComponentZ()[i] += newContainer->GetImagComponentZ()[i];
			i++;
			_volumeIterator->GetNextIndex();
		}
		delete newContainer;
		newContainer = nullptr;
	}
}

void ResultSinkVector3DComplexSum::PerformPostProcessing()
{
	for (auto postProcessor : _postProcessors)
	{
		postProcessor->PerformProstprocessing(_summedUpResult->GetRealComponentX(), _volumeIterator->GetVolumenSize());
		postProcessor->PerformProstprocessing(_summedUpResult->GetRealComponentY(), _volumeIterator->GetVolumenSize());
		postProcessor->PerformProstprocessing(_summedUpResult->GetRealComponentZ(), _volumeIterator->GetVolumenSize());
	}

}

const ResultContainerVector3DComplex * ResultSinkVector3DComplexSum::GetResultContainer(void) const
{
	return _summedUpResult;
}

index_t ResultSinkVector3DComplexSum::GetVolumeSize(void) const
{
	return _volumeIterator->GetVolumenSize();
}
