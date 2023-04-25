#include "SolverSettings/PortSettings.h"

PortSettings::PortSettings(std::string portName, std::string excitationTarget, std::string excitationAxis, std::string excitationSignal)
	: _portName(portName), _excitationTarget(excitationTarget),_excitationAxis(excitationAxis), _excitationSignalName(excitationSignal), _locationInCentre(true)
{
	if (_sourceTypeName.find(_excitationTarget) == _sourceTypeName.end()) { throw std::invalid_argument("Excitation axis not supported."); }
	if (_sourceTypeName.find(_excitationTarget) == _sourceTypeName.end()) { throw std::invalid_argument("Excitation type not supported."); }
}

void PortSettings::SetExcitationPoint(index_t x, index_t y, index_t z)
{
	_coordinateX = x;
	_coordinateY = y;
	_coordinateZ = z;
	_locationInCentre = false;
}


std::string PortSettings::Print()
{
	std::string settings =
		"Port " + _portName + ":\n" +
		"Exitation of " + _excitationAxis + " component of the " + _excitationTarget + "\n"
		"Excitation at: x=" + std::to_string(_coordinateX) + " y=" + std::to_string(_coordinateY) + " z=" + std::to_string(_coordinateZ) + "\n" +
		"Excitation signal: " + _excitationSignalName +"\n";
		
	return settings;
}
