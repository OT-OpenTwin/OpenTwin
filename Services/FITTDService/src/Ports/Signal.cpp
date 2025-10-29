// @otlicense

#include "Ports/Signal.h"


Signal & Signal::operator=(const Signal & rhs)
{
	if (this == &rhs)
	{
		return *this;
	}
	_signalName = rhs._signalName;
	_signalType = rhs._signalType;
	_minFrequency = rhs._minFrequency;
	_maxFrequency = rhs._maxFrequency;
	return *this;
}

std::string Signal::Print()
{
	std::string signalDescription =
		"Signal: " + _signalName + "\n" +
		"Signal type: " + _signalType + "\n" +
		"Max Frequency: " + _minFrequency.Print() + "\n" +
		"Min Frequency: " + _maxFrequency.Print() + "\n";
	return signalDescription;
}
