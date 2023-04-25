/*********************************************************************
 * @file   Signal.h
 * @brief  Container for properties of signal types. Frequencies are handled internally with the unit Herz.
 * 
 * @author Jan Wagner
 * @date   July 2022
 *********************************************************************/
#pragma once
#include <string>
#include <map>

#include "EntitySignalType.h"
#include "PhysicalQuantities.h"

enum signalType {sinusoidalGauss};

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
