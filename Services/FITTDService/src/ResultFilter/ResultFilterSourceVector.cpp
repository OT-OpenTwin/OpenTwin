// @otlicense

#include "ResultFilter/ResultFilterSinkVector.h"

void ResultFilterSinkVector::ExecuteFilter(int currentTimestep, double * input)
{
	for (auto filter : _finalFilter)
	{
		filter->ExecuteIfFrequencyMatches(currentTimestep, input);
	}

}