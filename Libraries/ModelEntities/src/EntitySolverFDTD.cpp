
#include "EntitySolverFDTD.h"
#include "OldTreeIcon.h"

#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntitySolverFDTD> registrar("EntitySolverFDTD");

EntitySolverFDTD::EntitySolverFDTD(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner) : EntitySolver(ID, parent, obs, ms, owner)
{
}

EntitySolverFDTD::~EntitySolverFDTD()
{
}

void EntitySolverFDTD::createProperties(std::string& _meshFolderName, ot::UID& _meshFolderID, std::string& _meshName, ot::UID& _meshID)
{
	getProperties().createProperty(new EntityPropertiesEntityList("Mesh", _meshFolderName, _meshFolderID, _meshName, _meshID), "General");

	// Boundary conditions value list
	std::list<std::string> boundaryValues{ "PEC", "PMC", "MUR", "PML_8" };
	// Excitation types lists
	std::list<std::string> excitationTypes{ "Gauss Excitation" };

	EntityPropertiesDouble::createProperty("Frequency", "Start Frequency", 0, "FDTDSolver", getProperties());
	EntityPropertiesDouble::createProperty("Frequency", "End Frequency", 0, "FDTDSolver", getProperties());
	EntityPropertiesInteger::createProperty("Simulation Settings", "Timesteps", 10000, "FDTDSolver", getProperties());
	EntityPropertiesSelection::createProperty("Simulation Settings", "Excitation type", excitationTypes, "Gauss Excitation", "FDTDSolver", getProperties());
	EntityPropertiesDouble::createProperty("Simulation Settings", "End Criteria", 1e-5, "FDTDSolver", getProperties());
	EntityPropertiesInteger::createProperty("Simulation Settings", "Oversampling", 6, "FDTDSolver", getProperties());
	EntityPropertiesSelection::createProperty("Boundary Conditions", "Xmin", boundaryValues, "PEC", "FDTDSolver", getProperties());
	EntityPropertiesSelection::createProperty("Boundary Conditions", "Xmax", boundaryValues, "PEC", "FDTDSolver", getProperties());
	EntityPropertiesSelection::createProperty("Boundary Conditions", "Ymin", boundaryValues, "PEC", "FDTDSolver", getProperties());
	EntityPropertiesSelection::createProperty("Boundary Conditions", "Ymax", boundaryValues, "PEC", "FDTDSolver", getProperties());
	EntityPropertiesSelection::createProperty("Boundary Conditions", "Zmin", boundaryValues, "PEC", "FDTDSolver", getProperties());
	EntityPropertiesSelection::createProperty("Boundary Conditions", "Zmax", boundaryValues, "PEC", "FDTDSolver", getProperties());
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
	
	/*EntityPropertiesSelection* problemType = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Problem type"));
	EntityPropertiesInteger* startFrequency = dynamic_cast<EntityPropertiesInteger*>(getProperties().getProperty("Start Frequency"));
	EntityPropertiesInteger* centerFrequency = dynamic_cast<EntityPropertiesInteger*>(getProperties().getProperty("Center Frequency"));
	EntityPropertiesInteger* endFrequency = dynamic_cast<EntityPropertiesInteger*>(getProperties().getProperty("End Frequency"));
	EntityPropertiesInteger* timestepSetting = dynamic_cast<EntityPropertiesInteger*>(getProperties().getProperty("Timesteps"));
	EntityPropertiesSelection* excitationType = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Ecitation type"));
	EntityPropertiesSelection* boundaryConditions = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Boundary Conditions"));*/
	
	getProperties().forceResetUpdateForAllProperties();

	return updatePropertiesGrid;
}
