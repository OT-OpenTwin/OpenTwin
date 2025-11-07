// @otlicense
// File: EntitySolverCircuitSimulator.cpp
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

#include "EntitySolverCircuitSimulator.h"
#include "OldTreeIcon.h"

#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntitySolverCircuitSimulator> registrar("EntitySolverCircuitSimulator");

EntitySolverCircuitSimulator::EntitySolverCircuitSimulator(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms) :
	EntitySolver(ID, parent, obs, ms)
{
}

EntitySolverCircuitSimulator::~EntitySolverCircuitSimulator()
{
}

void EntitySolverCircuitSimulator::createProperties(const std::string circuitFolderName, ot::UID circuitFolderID, const std::string circuitName, ot::UID circuitID)
{
	EntityPropertiesSelection::createProperty("Settings","Simulation Type", {".dc",".TRAN",".ac"}, ".dc", "default", getProperties());
	EntityPropertiesEntityList::createProperty("Settings", "Circuit", circuitFolderName, circuitFolderID, "", -1, "default", getProperties());
	

	createDCProperties();
	createTranProperties();
	createACProperties();
	

	SetVisibleDCSimulationParameters(true);
	SetVisibleTRANSimulationParameters(false);
	SetVisibleACSimulationParameters(false);
}


bool EntitySolverCircuitSimulator::SetVisibleDCSimulationParameters(bool visible)
{
	const bool isVisible = getProperties().getProperty("Element")->getVisible();
	const bool refresh = isVisible != visible;
	if (refresh)
	{
		getProperties().getProperty("Element")->setVisible(visible);
		getProperties().getProperty("From")->setVisible(visible);
		getProperties().getProperty("To")->setVisible(visible);
		getProperties().getProperty("Step")->setVisible(visible);
		this->setModified();
	}
	return refresh;
}

bool EntitySolverCircuitSimulator::SetVisibleTRANSimulationParameters(bool visible)
{
	const bool isVisible = getProperties().getProperty("Duration")->getVisible();
	const bool refresh = isVisible != visible;
	if (refresh)
	{
		getProperties().getProperty("Duration")->setVisible(visible);
		getProperties().getProperty("TimeSteps")->setVisible(visible);
		this->setModified();

	}
	return refresh;
}

bool EntitySolverCircuitSimulator::SetVisibleACSimulationParameters(bool visible)
{

	const bool isVisible = getProperties().getProperty("Variation")->getVisible();
	const bool refresh = isVisible != visible;
	if (refresh)
	{
		getProperties().getProperty("Variation")->setVisible(visible);
		getProperties().getProperty("Number of Points")->setVisible(visible);
		getProperties().getProperty("Starting Frequency")->setVisible(visible);
		getProperties().getProperty("Final Frequency")->setVisible(visible);
		this->setModified();

	}
	return refresh;
}



void EntitySolverCircuitSimulator::createDCProperties()
{
	EntityPropertiesEntityList* circuit = dynamic_cast<EntityPropertiesEntityList*>(getProperties().getProperty("Circuit"));
	std::string elementFolder = circuit->getValueName();
	ot::UID elementFolderID = circuit->getValueID();

	EntityPropertiesEntityList::createProperty("DC-Settings", "Element", elementFolder, elementFolderID, "", -1, "default", getProperties());

	
	//EntityPropertiesString::createProperty("DC-Settings", "Element","v1", "default", getProperties());
	EntityPropertiesString::createProperty("DC-Settings", "From", "0", "default", getProperties());
	EntityPropertiesString::createProperty("DC-Settings", "To", "100", "default", getProperties());
	EntityPropertiesString::createProperty("DC-Settings", "Step", "10", "default", getProperties());
}

void EntitySolverCircuitSimulator::createTranProperties()
{

	EntityPropertiesString::createProperty("TRAN-Settings", "Duration", "10ms", "default", getProperties());
	EntityPropertiesString::createProperty("TRAN-Settings", "TimeSteps", "1ms", "default", getProperties());
}

void EntitySolverCircuitSimulator::createACProperties()
{
	EntityPropertiesSelection::createProperty("AC-Properties", "Variation", { "dec","oct","lin" }, "lin", "default", getProperties());
	EntityPropertiesString::createProperty("AC-Properties", "Number of Points", "100", "default", getProperties());
	EntityPropertiesString::createProperty("AC-Properties", "Starting Frequency", "1", "default", getProperties());
	EntityPropertiesString::createProperty("AC-Properties", "Final Frequency", "100HZ", "default", getProperties());
}

bool EntitySolverCircuitSimulator::updateFromProperties()
{

	bool refresh = false;
	auto baseProperty = getProperties().getProperty("Simulation Type");
	auto selectionProperty = dynamic_cast<EntityPropertiesSelection*>(baseProperty);

	EntityPropertiesEntityList* circuit = dynamic_cast<EntityPropertiesEntityList*>(getProperties().getProperty("Circuit"));
	std::string elementFolder = circuit->getValueName();
	ot::UID elementFolderID = circuit->getValueID();
	
	EntityPropertiesEntityList* element = dynamic_cast<EntityPropertiesEntityList*>(getProperties().getProperty("Element"));
	if (element->getEntityContainerName() != elementFolder || element->getEntityContainerID() != elementFolderID)
	{
		element->setEntityContainerName(elementFolder);
		element->setEntityContainerID(elementFolderID);
		refresh = true;
	}
	

	if (selectionProperty->getValue() == ".dc")
	{
		refresh |= SetVisibleDCSimulationParameters(true);
		refresh |= SetVisibleTRANSimulationParameters(false);
		refresh |= SetVisibleACSimulationParameters(false);
	}
	else if(selectionProperty->getValue() == ".TRAN")
	{
		refresh |= SetVisibleDCSimulationParameters(false);
		refresh |= SetVisibleTRANSimulationParameters(true);
		refresh |= SetVisibleACSimulationParameters(false);
	}
	else
	{
		refresh |= SetVisibleDCSimulationParameters(false);
		refresh |= SetVisibleTRANSimulationParameters(false);
		refresh |= SetVisibleACSimulationParameters(true);

	}

	

	if (refresh)
	{
		getProperties().forceResetUpdateForAllProperties();
	}

	/*createDCProperties();
	createTranProperties();*/

	
	return refresh;
}