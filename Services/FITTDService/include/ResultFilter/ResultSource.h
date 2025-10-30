// @otlicense
// File: ResultSource.h
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
#include "Grid/GridIteratorVolume.h"
#include "Grid/IGridIteratorVolume.h"
#include "ResultFilter/ResultContainer.h"
#include "ResultHandling/ExecutionBarrier.h"

class ResultSource
{
public:
	~ResultSource();
	ResultSource(ExecutionBarrier * executionBarrier , IGridIteratorVolume * volumeIter);
	IGridIteratorVolume * GetSubvolumeIterator() const { return _subVolumeIterator; };
	int GetCurrentTimestep() const { return _currentTimestep; };

	bool ShallBeExecuted(int currentTimestep)
	{
		return _executionBarrier->AllowExecution(currentTimestep);
	}

	virtual ResultContainer * GenerateResultContainer(int currentTimeStep) = 0;

protected:
	IGridIteratorVolume * _volumeIterator = nullptr;
	IGridIteratorVolume * _subVolumeIterator = nullptr;

	int _currentTimestep;
private:
	ExecutionBarrier * _executionBarrier = nullptr;

};