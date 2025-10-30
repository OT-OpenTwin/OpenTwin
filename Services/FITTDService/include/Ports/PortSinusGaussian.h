// @otlicense
// File: PortSinusGaussian.h
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
#include "Port.h"
template <class T>
class PortSinusGaussian : public Port<T>
{
	using Port<T>::_index;
	using Port<T>::EFunc;

	double _frequencyMin =0;
	double _frequencyMax =0;

	double _frequencyMiddle = 0.;
	double _exponentCoefficient = 0.;
	double _timeShift = 0.;
	const double _fadedAmplitude = 0.00001;

	void CalculateCoefficients();
	T CalculateCurrentValue(float currentTime) override;

public:
	PortSinusGaussian(std::string portName, std::pair<Point3D,index_t> location , sourceType fieldType, axis sourceAxis, std::string signalName, double frequencyMin, double frequencyMax)
		: Port<T>(portName,location, fieldType, sourceAxis, signalName), _frequencyMin(frequencyMin), _frequencyMax(frequencyMax)
	{
		assert(_frequencyMin <= _frequencyMax);
		CalculateCoefficients();
	};
};

#include "Ports/PortSinusGaussian.hpp"