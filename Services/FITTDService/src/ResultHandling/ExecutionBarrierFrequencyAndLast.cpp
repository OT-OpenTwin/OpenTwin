// @otlicense

#include "ResultHandling\ExecutionBarrierFrequencyAndLast.h"

bool ExecutionBarrierFrequencyAndLast::AllowExecution(int currentTimestep)
{
	return currentTimestep % _executionFrequency == 0 || currentTimestep == _lastTimeStep;
}
