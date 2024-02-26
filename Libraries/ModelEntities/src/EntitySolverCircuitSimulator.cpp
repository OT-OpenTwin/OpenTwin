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
	//EntityPropertiesEntityList::createProperty("Circuit", "Circuit", circuitFolderName, circuitFolderID, circuitName, circuitID, "CircuitSimulator", getProperties());
	EntityPropertiesString::createProperty("Settings", "Simulation Type", ".dc 0 1 12V", "CircuitSimulator", getProperties());
	EntityPropertiesString::createProperty("Settings", "Circuit Name", "Circuit Simulator", "CircuitSimulator", getProperties());
}

bool EntitySolverCircuitSimulator::updateFromProperties(void)
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	// Here we need to update the plot (send a message to the visualization service)
	getProperties().forceResetUpdateForAllProperties();


	return false;
}