// @otlicense


#include "EntitySolverGetDP.h"
#include "OldTreeIcon.h"

#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntitySolverGetDP> registrar("EntitySolverGetDP");

EntitySolverGetDP::EntitySolverGetDP(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner) : EntitySolver(ID, parent, obs, ms, owner)
{
}

EntitySolverGetDP::~EntitySolverGetDP()
{
}

void EntitySolverGetDP::createProperties(std::string& _meshFolderName, ot::UID& _meshFolderID, std::string& _meshName, ot::UID& _meshID)
{
	getProperties().createProperty(new EntityPropertiesEntityList("Mesh", _meshFolderName, _meshFolderID, _meshName, _meshID), "General");
	EntityPropertiesSelection::createProperty("General", "Problem type", { "Electrostatics" }, "Electrostatics", "GetDPSolver", getProperties());

	// Create the electrostatics properties
	EntityPropertiesDouble::createProperty("Electrostatics", "Boundary potential", 0, "GetDPSolver", getProperties());



	// Add a debug switch
	EntityPropertiesBoolean::createProperty("Specials", "Debug", false, "GetDPSolver", getProperties());

	// Configure the visibility
	updateFromProperties();

	getProperties().forceResetUpdateForAllProperties();
}

bool EntitySolverGetDP::updateFromProperties(void)
{
	bool updatePropertiesGrid = false;

	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	// Check and update the visibility
	EntityPropertiesSelection* problemType = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Problem type"));

	bool boundaryPotentialVisible = problemType->getValue() == "Electrostatics";

	EntityPropertiesDouble* boundaryPotential = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Boundary potential"));

	if (boundaryPotentialVisible != boundaryPotential->getVisible()) updatePropertiesGrid = true;

	boundaryPotential->setVisible(boundaryPotentialVisible);

	getProperties().forceResetUpdateForAllProperties();

	return updatePropertiesGrid;
}
