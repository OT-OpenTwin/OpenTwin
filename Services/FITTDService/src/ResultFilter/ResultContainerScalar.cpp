#include "ResultFilter\ResultContainerScalar.h"

ResultContainerScalar::ResultContainerScalar(double * values, index_t size, int currentTimeStep) 
	: ResultContainer(currentTimeStep,size)
{
	_result = values;
}

ResultContainerScalar::~ResultContainerScalar()
{
	delete[] _result;
}

void ResultContainerScalar::ApplyFilter(ResultFilter * filter)
{
	filter->ExecuteIfFrequencyMatches(_currentTimeStep, _result);
}
