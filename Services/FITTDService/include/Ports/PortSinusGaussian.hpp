// @otlicense
// File: PortSinusGaussian.hpp
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
#include "Ports/PortSinusGaussian.h"
#define _USE_MATH_DEFINES
#include <math.h>


template<class T>
inline void PortSinusGaussian<T>::CalculateCoefficients()
{
	_frequencyMiddle = (_frequencyMax + _frequencyMin) / 2;
	_exponentCoefficient = -pow(M_PI, 2) / (4 * log(_fadedAmplitude)) * pow(_frequencyMax - _frequencyMin, 2);
	_timeShift = sqrt(-(log(_fadedAmplitude)/_exponentCoefficient));
}

template<class T>
inline T PortSinusGaussian<T>::CalculateCurrentValue(float currentTime)
{
	double t1 = exp(-_exponentCoefficient * pow(currentTime - _timeShift, 2));
	double t2 = cos(2 * M_PI * _frequencyMiddle * (currentTime - _timeShift));
	T currentValue =static_cast<T>(t1 * t2);
	return currentValue; 
}