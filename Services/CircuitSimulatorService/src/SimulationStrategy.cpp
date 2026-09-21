// @otlicense
// File: SimulationStrategy.cpp
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

// Open Twin Header
#include "OTCore/Logging/Logger.h"

// Service Header
#include "SimulationStrategy.h"
#include "Application.h"

std::string DCSimulationStrategy::getVoltageSourceNetlistType(VoltageSource* _voltageSource) const
{
	_voltageSource->setType("DC");
	return _voltageSource->getType() + " ";
}

std::string DCSimulationStrategy::generateSimulationLine(EntityBase* _solverEntity, const ElementNamingRegistry& _elementNamingRegistry) const
{
	EntityPropertiesBase* elementsProperty = _solverEntity->getProperties().getProperty("Element");
	if(!elementsProperty)
	{
		OT_LOG_E("Failed to retrieve the 'Elements' property from the solver entity.");
		return "failed";
	}

	auto* elementListProperty = dynamic_cast<EntityPropertiesEntityList*>(elementsProperty);
	if(!elementListProperty)
	{
		OT_LOG_E("Failed to retrieve the 'Elements' property from the solver entity.");
		return "failed";
	}

	std::string element = Application::instance()->extractStringAfterDelimiter(elementListProperty->getValueName(), '/', 2);
	if (element == "failed") {
		OT_LOG_E("No Element for DC Simulation found or selected!");
		return "failed";
	}

	std::string netlistName = _elementNamingRegistry.getNetlistName(element);
	EntityPropertiesBase* fromProperty = _solverEntity->getProperties().getProperty("From");
	EntityPropertiesBase* toProperty = _solverEntity->getProperties().getProperty("To");
	EntityPropertiesBase* stepProperty = _solverEntity->getProperties().getProperty("Step");
	if(fromProperty == nullptr || toProperty == nullptr || stepProperty == nullptr) 
	{
		OT_LOG_E("Failed to retrieve one or more properties from the solver entity.");
		return "failed";
	}

	auto* from = dynamic_cast<EntityPropertiesString*>(fromProperty);
	auto* to = dynamic_cast<EntityPropertiesString*>(toProperty);
	auto* step = dynamic_cast<EntityPropertiesString*>(stepProperty);
	if(!from || !to || !step) 
	{
		OT_LOG_E("One or more properties are not of type EntityPropertiesString.");
		return "failed";
	}


	return ".dc " + netlistName + " " + from->getValue() + " " + to->getValue() + " " + step->getValue();
}

std::string ACSimulationStrategy::getVoltageSourceNetlistType(VoltageSource* _voltageSource) const
{
	_voltageSource->setType("AC");
	return "DC 0 AC " + _voltageSource->getAmplitude();
}

std::string ACSimulationStrategy::generateSimulationLine(EntityBase* _solverEntity, const ElementNamingRegistry& _elementNamingRegistry) const
{
	EntityPropertiesBase* variationProperty = _solverEntity->getProperties().getProperty("Variation");
	EntityPropertiesBase* npProperty = _solverEntity->getProperties().getProperty("Number of Points");
	EntityPropertiesBase* fStartProperty = _solverEntity->getProperties().getProperty("Starting Frequency");
	EntityPropertiesBase* fEndProperty = _solverEntity->getProperties().getProperty("Final Frequency");

	if(variationProperty == nullptr || npProperty == nullptr || fStartProperty == nullptr || fEndProperty == nullptr) {
		OT_LOG_E("Failed to retrieve one or more properties from the solver entity.");
		return "failed";
	}

	auto* variation = dynamic_cast<EntityPropertiesSelection*>(variationProperty);
	auto* np = dynamic_cast<EntityPropertiesString*>(npProperty);
	auto* fStart = dynamic_cast<EntityPropertiesString*>(fStartProperty);
	auto* fEnd = dynamic_cast<EntityPropertiesString*>(fEndProperty);

	if(!variation || !np || !fStart || !fEnd) {
		OT_LOG_E("One or more properties are not of the expected type.");
		return "failed";
	}

	return ".ac " + variation->getValue() + " " + np->getValue() + " " + fStart->getValue() + " " + fEnd->getValue();
}

std::string TRANSimulationStrategy::getVoltageSourceNetlistType(VoltageSource* _voltageSource) const
{
	_voltageSource->setType("TRAN");
	return _voltageSource->getFunction();
}

std::string TRANSimulationStrategy::generateSimulationLine(EntityBase* _solverEntity, const ElementNamingRegistry& _elementNamingRegistry) const
{
	EntityPropertiesBase* durationProperty = _solverEntity->getProperties().getProperty("Duration");
	EntityPropertiesBase* timeStepsProperty = _solverEntity->getProperties().getProperty("TimeSteps");

	if(durationProperty == nullptr || timeStepsProperty == nullptr) {
		OT_LOG_E("Failed to retrieve one or more properties from the solver entity.");
		return "failed";
	}

	auto* duration = dynamic_cast<EntityPropertiesString*>(durationProperty);
	auto* timeSteps = dynamic_cast<EntityPropertiesString*>(timeStepsProperty);

	if(!duration || !timeSteps) {
		OT_LOG_E("One or more properties are not of the expected type.");
		return "failed";
	}

	return ".TRAN " + timeSteps->getValue() + " " + duration->getValue();
}

std::unique_ptr<SimulationStrategy> createSimulationStrategy(const std::string& _simulationType)
{
	if(_simulationType == ".dc") 
	{
		return std::make_unique<DCSimulationStrategy>();
	} 
	else if(_simulationType == ".TRAN") 
	{
		return std::make_unique<TRANSimulationStrategy>();
	} 
	else 
	{
		return std::make_unique<ACSimulationStrategy>();
	}
}
