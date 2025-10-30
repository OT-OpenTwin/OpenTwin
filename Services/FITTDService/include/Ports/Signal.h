// @otlicense
// File: Signal.h
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
#include <string>
#include <map>

#include "EntitySignalType.h"
#include "PhysicalQuantities.h"

enum signalType {sinusoidalGauss};

//! @brief Container for properties of signal types. Frequencies are handled internally with the unit Herz.
class Signal
{
public:
	Signal(std::string signalName, std::string type, PhysicalQuantities minFrequency, PhysicalQuantities maxFrequency)
		: _signalName(signalName), _signalType(type), _minFrequency(minFrequency), _maxFrequency(maxFrequency){};
	Signal() {};
	Signal & operator=(const Signal& rhs);

	const double GetFrequencyMin() const { return _minFrequency.GetInternalValue() ; }
	const double GetFrequencyMax() const { return _maxFrequency.GetInternalValue() ; }
	const signalType GetSignalType() const { return	types.at(_signalType); }
	const std::string GetSignalTypeName() const { return _signalType; }
	const std::string GetSignalName() const { return _signalName; }

	std::string Print();

private:
	std::map<std::string, signalType> types = { {EntitySignalType::GetValueSinGuasSignal(), sinusoidalGauss} };

	std::string _signalType;
	std::string _signalName;
	PhysicalQuantities _minFrequency;
	PhysicalQuantities _maxFrequency;
};
