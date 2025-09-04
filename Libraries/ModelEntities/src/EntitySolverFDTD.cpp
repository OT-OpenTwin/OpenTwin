
#include "EntitySolverFDTD.h"
#include "OldTreeIcon.h"

#include <bsoncxx/builder/basic/array.hpp>

EntitySolverFDTD::EntitySolverFDTD(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner) : EntitySolver(ID, parent, obs, ms, factory, owner)
{
}

EntitySolverFDTD::~EntitySolverFDTD()
{
}

void EntitySolverFDTD::createProperties(std::string& _meshFolderName, ot::UID& _meshFolderID, std::string& _meshName, ot::UID& _meshID)
{
	getProperties().createProperty(new EntityPropertiesEntityList("Mesh", _meshFolderName, _meshFolderID, _meshName, _meshID), "General");

	EntityPropertiesInteger::createProperty("Frequency", "Start Frequency", 0, "FDTDSolver", getProperties());
	EntityPropertiesInteger::createProperty("Frequency", "Center Frequency", 0, "FDTDSolver", getProperties());
	EntityPropertiesInteger::createProperty("Frequency", "End Frequency", 0, "FDTDSolver", getProperties());
	EntityPropertiesInteger::createProperty("Simulation Settings", "Timesteps", 10000, "FDTDSolver", getProperties());
	EntityPropertiesInteger::createProperty("Simulation Settings", "Timesteps", 10000, "FDTDSolver", getProperties());
	EntityPropertiesSelection::createProperty("Simulation Settings", "Excitation type", { "Gauss Excitation", "" }, "", "FDTDSolver", getProperties());
	EntityPropertiesInteger::createProperty("Simulation Settings", "Boundary Conditions", 0, "FDTDSolver", getProperties());
	EntityPropertiesBoolean::createProperty("Specials", "Debug", false, "FDTDSolver", getProperties());


	// Configure the visibility
	updateFromProperties();

	getProperties().forceResetUpdateForAllProperties();
}

bool EntitySolverFDTD::updateFromProperties(void)
{
	bool updatePropertiesGrid = false;

	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	// Check and update the visibility
	//
	//EntityPropertiesSelection* problemType = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Problem type"));
	//EntityPropertiesInteger* startFrequency = dynamic_cast<EntityPropertiesInteger*>(getProperties().getProperty("Start Frequency"));
	//EntityPropertiesInteger* centerFrequency = dynamic_cast<EntityPropertiesInteger*>(getProperties().getProperty("Center Frequency"));
	//EntityPropertiesInteger* endFrequency = dynamic_cast<EntityPropertiesInteger*>(getProperties().getProperty("End Frequency"));
	//EntityPropertiesInteger* timestepSetting = dynamic_cast<EntityPropertiesInteger*>(getProperties().getProperty("Timesteps"));
	//

	getProperties().forceResetUpdateForAllProperties();

	return updatePropertiesGrid;
}
