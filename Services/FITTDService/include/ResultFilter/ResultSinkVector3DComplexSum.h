// @otlicense

#pragma once
#include "ResultFilter/ResultSink.h"
#include "ResultContainerVector3DComplex.h"

class ResultSinkVector3DComplexSum : public ResultSink
{
public:
	ResultSinkVector3DComplexSum(int executionFrequency, int totalTimeSteps)
		:ResultSink(executionFrequency, totalTimeSteps) {};
	virtual ~ResultSinkVector3DComplexSum();

	void ConsumeResultContainer(ResultContainer * container) override;
	virtual void PerformPostProcessing() override;

	const ResultContainerVector3DComplex * GetResultContainer(void) const;
	index_t GetVolumeSize(void) const;

private:
	ResultContainerVector3DComplex * _summedUpResult = nullptr;
};