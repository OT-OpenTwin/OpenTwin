// @otlicense
// File: PortSettings.h
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

#include <map>
#include "EntitySolverPort.h"

#include <string>
#include "Ports/SourceTarget.h"
#include "Ports/Signal.h"
#include "SystemDependencies/SystemDependentDefines.h"

//! @brief Contains information about a single port. Mirrors the settings that were selected for the corresponding entity in the UI.
class PortSettings
{
public:
	PortSettings & operator=(PortSettings &other) = delete;

	/**
	 * @brief  .
	 * 
	 * @param portName
	 * @param excitationTarget
	 * @param excitationAxis
	 * @param excitationSignal
	 * @throws invalid_argument if excitation axis or target are not included in map.
	 */
	PortSettings(std::string portName, std::string excitationTarget, std::string excitationAxis, std::string excitationSignal);

	void SetExcitationPoint(index_t x, index_t y, index_t z);

	const index_t GetCoorX() const {return _coordinateX;};
	const index_t GetCoorY() const {return _coordinateY;};
	const index_t GetCoorZ() const {return _coordinateZ;};
	
	const bool LocationIsInCentre() const { return _locationInCentre; }

	const std::string GetPortName() const { return _portName; }

	const sourceType GetSourceType()const 
	{ 
		return _sourceTypeName.at(_excitationTarget); 
	}

	const axis GetAxis()const 
	{ 
		return _sourceAxisName.at(_excitationAxis); 
	}

	const std::string GetExcitationSignalName(void) { return _excitationSignalName; }

	void SetExcitationSignal(Signal excitationSignal) { _excitationSignal = excitationSignal; }
	const Signal GetExcitationSignal(void) { return _excitationSignal; }

	std::string Print();

private:
	std::string _portName;

	index_t _coordinateX;
	index_t _coordinateY;
	index_t _coordinateZ;

	std::string _excitationTarget;
	std::string _excitationAxis;
	std::string _excitationSignalName;

	Signal _excitationSignal;

	bool _locationInCentre = true;

	index_t _index;
	std::string _specializedPortProperties;

	std::map<std::string, sourceType> _sourceTypeName{ {EntitySolverPort::GetValueExcitationTargetE() ,e }, {EntitySolverPort::GetValueExcitationTargetH(),h} };
	std::map<std::string, axis> _sourceAxisName{ {EntitySolverPort::GetValueExcitationAxisX(),x_axis }, {EntitySolverPort::GetValueExcitationAxisY(),y_axis },{EntitySolverPort::GetValueExcitationAxisZ(),z_axis } };

};