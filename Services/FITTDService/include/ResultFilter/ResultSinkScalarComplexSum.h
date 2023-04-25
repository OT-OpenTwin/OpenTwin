#pragma once
#include "ResultFilter/ResultSink.h"
#include "ResultContainerScalarComplex.h"

class ResultSinkScalarComplexSum : public ResultSink
{
public:
	ResultSinkScalarComplexSum(int executionFrequency, int totalTimeSteps)
		:ResultSink(executionFrequency, totalTimeSteps) {};
	virtual ~ResultSinkScalarComplexSum();

	void ConsumeResultContainer(ResultContainer * container) override;
	ResultContainerScalarComplex * GetResultContainer() { return _summedUpResult; };
	virtual void PerformPostProcessing() override;

private:
	ResultContainerScalarComplex * _summedUpResult = nullptr;

};