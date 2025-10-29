// @otlicense

#pragma once
#include "ExecutionBarrier.h"

class ExecutionBarrierFrequencyAndLast : public ExecutionBarrier
{
public:
	ExecutionBarrierFrequencyAndLast(int frequency, int lastTimeStep) :
		_executionFrequency(frequency), _lastTimeStep(lastTimeStep) {}
	virtual bool AllowExecution(int currentTimestep) override;

private:
	int _executionFrequency;
	int _lastTimeStep;
};

