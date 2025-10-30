// @otlicense
// File: ResultFilter.h
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
#include "Grid/IGridIteratorVolume.h"
#include <vector>


class ResultFilter
{
public:
	ResultFilter(int executionFrequency, int totalTimeSteps)
		: _executionFrequency(executionFrequency), _lastTimeStep(totalTimeSteps - 1) {};
	virtual ~ResultFilter() {};
	void ExecuteIfFrequencyMatches(int currentTimestep, double * input);
	bool FrequencyMatches(int currentTimestep);
	virtual void SetVolumeIterator(IGridIteratorVolume *  volumenFilter) = 0;
	const IGridIteratorVolume* GetVolumeIterator() { return _volumeIterator; };

protected:
	int _lastTimeStep = 0;
	IGridIteratorVolume *  _volumeIterator = nullptr;
	virtual void ExecuteFilter(int currentTimestep, double * input) = 0;

private:
	int _executionFrequency = 0;
};

