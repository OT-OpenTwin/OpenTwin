// @otlicense
// File: ResultFilterDFT.cpp
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

#define _USE_MATH_DEFINES
#include <math.h> 

#include "ResultFilter/ResultFilterDFT.h"

void ResultFilterDFT::ExecuteFilter(int currentTimestep, double * input)
{
	_volumeIterator->Reset();
	CoefficientsUpdate(currentTimestep);

	int i = 0;
	input[i] *= _realCoefficient;
	input[i + _offsetToComplex] *= _imagCoefficient;

	while (_volumeIterator->HasNext())
	{
		i++;
		_volumeIterator->GetNextIndex();

		input[i] *= _realCoefficient;
		input[i + _offsetToComplex] = input[i] *_imagCoefficient;
	}
}

ResultFilterDFT::ResultFilterDFT(int executionFrequency, int totalTimeSteps, double observedFrequency)
	: ResultFilter(executionFrequency, totalTimeSteps), _frequency(observedFrequency)
{};


void ResultFilterDFT::CoefficientsUpdate(int currentTimestep)
{
	if (_currentTimestep < currentTimestep)
	{
		_currentTimestep = currentTimestep;

		_realCoefficient = cos(2 * M_PI * _frequency * currentTimestep / (_lastTimeStep + 1));
		_imagCoefficient = sin(2 * M_PI * _frequency * currentTimestep / (_lastTimeStep + 1));
	}
}