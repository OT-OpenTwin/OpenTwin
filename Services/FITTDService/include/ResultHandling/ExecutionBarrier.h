// @otlicense

#pragma once

class ExecutionBarrier
{
public:
	virtual bool AllowExecution(int currentTimestep) = 0;
};
