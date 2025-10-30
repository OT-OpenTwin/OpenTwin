// @otlicense
// File: ResultFilterSinkPointOverTime.cpp
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

#include "ResultFilter/ResultFilterSinkPointOverTime.h"


ResultFilterSinkPointOverTime::ResultFilterSinkPointOverTime(int executionFrequency, int totalTimeSteps, std::vector<ResultFilter*> finalFilter, double deltaT)
: ResultFilterSink(executionFrequency, totalTimeSteps, finalFilter), _deltaT(deltaT)
{
	_resultCategorie = scalar;
	_timeSteps.reserve(GetNumberOfExecutions());
}

void ResultFilterSinkPointOverTime::SetVolumeIterator(IGridIteratorVolume * volumenFilter)
{
	ResultFilterSink::SetVolumeIterator(volumenFilter);
	assert(_volumenIterator->GetVolumenSize() == 1);
	int collectionSize = GetNumberOfExecutions() * _volumenIterator->GetVolumenSize();
	_resultCollection.reserve(collectionSize);
}


void ResultFilterSinkPointOverTime::ExecuteFilter(int currentTimestep, double * input)
{
	_timeSteps.push_back(currentTimestep * _deltaT);
	for (int i = 0; i < _volumenIterator->GetVolumenSize(); i++)
	{
		_resultCollection.push_back(input[i]);
	}
	for (auto filter : _finalFilter)
	{
		filter->ExecuteIfFrequencyMatches(currentTimestep, _resultCollection.data());
	}
}