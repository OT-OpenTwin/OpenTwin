// @otlicense

#pragma once
#include "ResultFilter/ResultSinkScalarComplexSum.h"

ResultSinkScalarComplexSum::~ResultSinkScalarComplexSum()
{
	if (_summedUpResult != nullptr)
	{
		delete _summedUpResult;
		_summedUpResult = nullptr;
	}
}

void ResultSinkScalarComplexSum::ConsumeResultContainer(ResultContainer * container)
{
	ResultContainerScalarComplex * newContainer = dynamic_cast<ResultContainerScalarComplex*>(container);
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
			_summedUpResult->GetResult()[i] += newContainer->GetResult()[i];
			_summedUpResult->GetResultImag()[i] += newContainer->GetResult()[i];

			i++;
			_volumeIterator->GetNextIndex();
		}
		delete newContainer;
		newContainer = nullptr;
	}
}

void ResultSinkScalarComplexSum::PerformPostProcessing()
{
}
