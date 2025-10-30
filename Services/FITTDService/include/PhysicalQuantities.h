// @otlicense
// File: PhysicalQuantities.h
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

class PhysicalQuantities
{
public:
	PhysicalQuantities(double enteredValue, double internalValue, std::string selectedUnit)
		:_enteredValue(enteredValue), _internalValue(internalValue), _selectedUnit(selectedUnit) {};
	PhysicalQuantities() {};
	PhysicalQuantities & operator=(const PhysicalQuantities& rhs)
	{
		if(this == &rhs)
		{
			return *this;
		}
		_enteredValue = rhs._enteredValue;
		_internalValue = rhs._internalValue;
		_selectedUnit = rhs._selectedUnit;
		return *this;
	}

	std::string Print(void) { return std::to_string(_enteredValue) + " " + _selectedUnit; }
	const double GetInternalValue(void) const { return _internalValue; };
private:
	double _enteredValue;
	double _internalValue;
	std::string _selectedUnit;
};
