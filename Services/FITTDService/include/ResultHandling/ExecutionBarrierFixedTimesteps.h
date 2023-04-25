#pragma once
#include "ExecutionBarrier.h"
#include <list>

class ExecutionBarrierFixedTimesteps : public ExecutionBarrier
{
public:
	ExecutionBarrierFixedTimesteps(std::list<int> timeSteps)
		: _executionTimeSteps(timeSteps){}

	virtual bool AllowExecution(int currentTimestep) override;
private:
	std::list<int> _executionTimeSteps;
};
