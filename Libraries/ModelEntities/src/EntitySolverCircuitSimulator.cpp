#include "EntitySolverCircuitSimulator.h"
#include "Types.h"

#include <bsoncxx/builder/basic/array.hpp>

EntitySolverCircuitSimulator::EntitySolverCircuitSimulator(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner) :
	EntitySolver(ID, parent, obs, ms, factory, owner)
{
}

EntitySolverCircuitSimulator::~EntitySolverCircuitSimulator()
{
}

void EntitySolverCircuitSimulator::createProperties(const std::string circuitFolderName, ot::UID circuitFolderID, const std::string circuitName, ot::UID circuitID)
{
	EntityPropertiesSelection::createProperty("Settings","Simulation Type", {".dc",".TRAN"},".dc","default",getProperties());
	EntityPropertiesEntityList::createProperty("Settings", "Circuit", circuitFolderName, circuitFolderID, "", -1, "default", getProperties());
	EntityPropertiesString::createProperty("Settings", "Print Settings", "print all", "CircuitSimulator", getProperties());

	createDCProperties();
	createTranProperties();
	

	SetVisibleDCSimulationParameters(true);
	SetVisibleTRANSimulationParameters(false);
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

void EntitySolverCircuitSimulator::createDCProperties()
{
	EntityPropertiesEntityList* circuit = dynamic_cast<EntityPropertiesEntityList*>(getProperties().getProperty("Circuit"));
	std::string elementFolder = circuit->getValueName();
	ot::UID elementFolderID = circuit->getValueID();

	
	EntityPropertiesString::createProperty("DC-Settings", "Element","v1", "default", getProperties());
	EntityPropertiesString::createProperty("DC-Settings", "From", "0", "default", getProperties());
	EntityPropertiesString::createProperty("DC-Settings", "To", "100", "default", getProperties());
	EntityPropertiesString::createProperty("DC-Settings", "Step", "10", "default", getProperties());
}

void EntitySolverCircuitSimulator::createTranProperties()
{
	EntityPropertiesEntityList* circuit = dynamic_cast<EntityPropertiesEntityList*>(getProperties().getProperty("Circuit"));
	std::string elementFolder = circuit->getValueName();
	ot::UID elementFolderID = circuit->getValueID();

	EntityPropertiesString::createProperty("TRAN-Settings", "Duration", "10ms", "default", getProperties());
	EntityPropertiesString::createProperty("TRAN-Settings", "TimeSteps", "1ms", "default", getProperties());
}



bool EntitySolverCircuitSimulator::updateFromProperties()
{
	auto baseProperty = getProperties().getProperty("Simulation Type");
	auto selectionProperty = dynamic_cast<EntityPropertiesSelection*>(baseProperty);



	bool refresh = false;
	if (selectionProperty->getValue() == ".dc")
	{
		refresh = SetVisibleDCSimulationParameters(true);
		refresh |= SetVisibleTRANSimulationParameters(false);
	}
	else
	{
		refresh = SetVisibleDCSimulationParameters(false);
		refresh |= SetVisibleTRANSimulationParameters(true);
	}

	

	if (refresh)
	{
		getProperties().forceResetUpdateForAllProperties();
	}

	/*createDCProperties();
	createTranProperties();*/

	
	return refresh;
}