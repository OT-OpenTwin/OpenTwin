// @otlicense
// File: ResultSink.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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