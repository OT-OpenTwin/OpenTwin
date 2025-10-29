// @otlicense

#include "..\..\include\ResultHandling\ExecutionBarrierFixedTimesteps.h"

bool ExecutionBarrierFixedTimesteps::AllowExecution(int currentTimestep)
{
	bool allow = false;
	auto it = std::find(_executionTimeSteps.begin(), _executionTimeSteps.end(), currentTimestep);
	allow = it != _executionTimeSteps.end();
	return allow;
}
