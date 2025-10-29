// @otlicense

#pragma once
#include "ResultFilter/ResultSink.h"
#include "ResultFilter/ResultContainerScalar.h"
#include <vector>

class ResultSinkScalarAccumalating : public ResultSink
{
public:
	ResultSinkScalarAccumalating(int executionFrequency, int totalTimeSteps, double deltaT)
		:ResultSink(executionFrequency, totalTimeSteps), _deltaT(deltaT) {};

	void SetVolumeIterator(IGridIteratorVolume * volumeIter) override;
	void ConsumeResultContainer(ResultContainer * container) override;

	const double * GetResult() const { return _result.data(); };
	const double * GetTimesteps() const { return _timeSteps.data(); };
	const index_t GetResultContainerSize() const { return _result.size(); };

	virtual void PerformPostProcessing() override;

private:
	double _deltaT;
	std::vector<double> _timeSteps;
	std::vector<double> _result;

};