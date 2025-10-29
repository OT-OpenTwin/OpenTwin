// @otlicense

#pragma once
#include "ResultFilter/ResultSink.h"
#include "ResultFilter/ResultContainerVector3DComplex.h"
#include <vector>


class ResultSinkVector3DAccumalating : public ResultSink
{
public:
	ResultSinkVector3DAccumalating(int executionFrequency, int totalTimeSteps, double deltaT)
		:ResultSink(executionFrequency, totalTimeSteps), _deltaT(deltaT) {};

	void SetVolumeIterator(IGridIteratorVolume * volumeIter) override;
	void ConsumeResultContainer(ResultContainer * container) override;

	const double * GetResultX() const { return _resultX.data(); };
	const double * GetResultY() const { return _resultY.data(); };
	const double * GetResultZ() const { return _resultZ.data(); };
	const double * GetTimesteps() const { return _timeSteps.data(); };
	const index_t GetResultContainerSize() const { return _resultX.size(); };

	virtual void PerformPostProcessing() override;

private:
	double _deltaT;
	std::vector<double> _timeSteps;
	std::vector<double> _resultX;
	std::vector<double> _resultY;
	std::vector<double> _resultZ;
};