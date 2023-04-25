#pragma once
#include "ResultFilter/ResultFilter.h"


void ResultFilter::ExecuteIfFrequencyMatches(int currentTimestep, double * input)
{
	if (FrequencyMatches(currentTimestep))
	{
		ExecuteFilter(currentTimestep, input);
	}
}

bool ResultFilter::FrequencyMatches(int currentTimestep)
{
	return  currentTimestep % _executionFrequency == 0 || currentTimestep == _lastTimeStep;
}
