// @otlicense

/*********************************************************************
 * @file   PortSettings.h
 * @brief  Contains information about a single port. Mirrors the settings that were selected for the corresponding entity in the UI.
 * 
 * @author Jan Wagner
 * @date   July 2022
 *********************************************************************/
#pragma once
#include <map>
#include "EntitySolverPort.h"

#include <string>
#include "Ports/SourceTarget.h"
#include "Ports/Signal.h"
#include "SystemDependencies/SystemDependentDefines.h"

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