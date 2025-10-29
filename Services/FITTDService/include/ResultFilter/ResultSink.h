// @otlicense

#pragma once
#include "ResultFilter/ResultFilter.h"
#include "ResultFilter/ResultContainer.h"
#include "ResultPostProcessors/ResultPostprocessors.h"

#include <cmath>

class ResultSink 
{
public:
	ResultSink(int executionFrequency, int totalTimeSteps)
		: _executionFrequency(executionFrequency), _totalTimesteps(totalTimeSteps)
	{
		_nbOfExecutions = static_cast<int>(std::ceil(_totalTimesteps / static_cast<double>(executionFrequency)));
	};
	virtual ~ResultSink() 
	{
		for (auto postProcessor : _postProcessors)
		{
			delete postProcessor;
			postProcessor = nullptr;
		}
	};

	virtual void ConsumeResultContainer(ResultContainer * container)=0;
	virtual void SetVolumeIterator(IGridIteratorVolume * volumeIter) { _volumeIterator = volumeIter; };
	const int GetNbOfExectutions() const { return _nbOfExecutions; };
	virtual void PerformPostProcessing() =0;
	void AddPostProcessor(ResultPostProcessor * postProcessor) { _postProcessors.push_back(postProcessor); };

protected:
	std::vector<ResultPostProcessor *> _postProcessors;
	int _executionFrequency;
	int _totalTimesteps;
	int _nbOfExecutions;
	IGridIteratorVolume * _volumeIterator = nullptr;
};