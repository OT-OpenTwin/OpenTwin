// @otlicense
// File: ResultPipelineTimeDomain1Component.h
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

#include "ResultPipeline/ResultPipeline.h"
#include "Simulation/DegreesOfFreedom3DLoopCPU.h"
#include "SystemDependencies/SystemDependentDefines.h"
#include <vector>

template <class T>
class ResultPipelineTimeDomain1Component : public ResultPipeline
{
private:
	DegreesOfFreedom3DLoopCPU<T> &_doF;
	ResultVolumeFilter<T> * _filter;
	std::vector<T> _result;
	std::vector<T> _timeSteps;
	
	T*(DegreesOfFreedom3DLoopCPU<T>::*GetterOfDoF)(index_t index) const;
public:
	ResultPipelineTimeDomain1Component(int executionFrequency, int totalTimeSteps, DegreesOfFreedom3DLoopCPU<T> & doF, T*(DegreesOfFreedom3DLoopCPU<T>::*GetterDoF)(index_t index) const, ResultVolumeFilter<T> * filter)
		: ResultPipeline(executionFrequency, totalTimeSteps),  _doF(doF),  _filter(filter) , GetterOfDoF(GetterDoF)
	{
		_result.reserve(_lastTimeStep + 1);
		_timeSteps.reserve(_lastTimeStep + 1);

	};
	~ResultPipelineTimeDomain1Component()
	{
		delete _filter;
		_filter = nullptr;
	}
	const std::vector<T> & GetResultVector() const { return _result; };
	const std::vector<T> & GetTimeSteps() const { return _timeSteps; };

	virtual void Execute(int currentTimestep) override
	{
		_timeSteps.push_back(static_cast<T>(currentTimestep));
		auto temp = (_doF.*GetterOfDoF)(0);
		const T* result = _filter->FilterVolume(temp);
		_result.push_back(result[0]);
	}
};