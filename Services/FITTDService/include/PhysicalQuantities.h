// @otlicense

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
