// @otlicense
// File: PortGaussian.h
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
#include "Ports/Port.h"
template <class T>
class PortGaussian : public Port<T>
{
	using Port<T>::_index;
	using Port<T>::EFunc;

	int _middlePoint;
	int _deviation;
	T CalculateCurrentValue(float currentTime) override;
	
public:
	PortGaussian(std::string portName, std::pair<Point3D, index_t> &location, sourceType fieldType, axis sourceAxis, std::string signalName, int middlePoint, int deviation)
		: Port<T>(portName, location, fieldType, sourceAxis, signalName), _middlePoint(middlePoint) , _deviation(deviation)
	{
		//std::string specializedSettings = "Gaussian exitation with middlepoint: " + std::to_string(_middlePoint) +
		//" and deviation: " + std::to_string(_deviation);
		//CreateSettings(specializedSettings);
	};
};

#include "Ports/PortGaussian.hpp"