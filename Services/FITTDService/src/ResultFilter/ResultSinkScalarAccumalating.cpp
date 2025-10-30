// @otlicense
// File: ResultSinkScalarAccumalating.cpp
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

#include "ResultFilter/ResultSinkScalarAccumalating.h"

void ResultSinkScalarAccumalating::ConsumeResultContainer(ResultContainer * container)
{
	auto newContainer = dynamic_cast<ResultContainerScalar*>(container);
	assert(newContainer != nullptr);
	
	_timeSteps.push_back(newContainer->GetCurrentTimestep()*_deltaT);
	int i = 0;
	_volumeIterator->Reset();
	_result.push_back(newContainer->GetResult()[i]);
	while (_volumeIterator->HasNext())
	{
		i++;
		_volumeIterator->GetNextIndex();
		_result.push_back(newContainer->GetResult()[i]);
	}
	delete newContainer;
	newContainer = nullptr;
}

void ResultSinkScalarAccumalating::PerformPostProcessing()
{
}

void ResultSinkScalarAccumalating::SetVolumeIterator(IGridIteratorVolume * volumeIter)
{
	ResultSink::SetVolumeIterator(volumeIter);
	_timeSteps.reserve(_nbOfExecutions);
	_result.reserve(_nbOfExecutions * _volumeIterator->GetVolumenSize());
}