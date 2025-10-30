// @otlicense
// File: PortSettings.cpp
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
