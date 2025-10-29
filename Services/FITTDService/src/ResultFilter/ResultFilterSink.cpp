// @otlicense

#include "ResultFilter/ResultFilterSink.h"


ResultFilterSink::ResultFilterSink(int executionFrequency, int totalTimeSteps, std::vector<ResultFilter*> finalFilter = std::vector<ResultFilter*>()) 
	:ResultFilter(executionFrequency, totalTimeSteps), _finalFilter(finalFilter)
{
	_numberOfExecutions = static_cast<int>(std::ceil(totalTimeSteps / static_cast<float>(executionFrequency)));
}

void ResultFilterSink::SetVolumeIterator(IGridIteratorVolume * volumenFilter)
{
	_volumenIterator = volumenFilter;
}
