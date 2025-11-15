// @otlicense
// File: EntitySolverPyrit.cpp
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

#include "EntitySolverPyrit.h"

#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntitySolverPyrit> registrar("EntitySolverPyrit");

EntitySolverPyrit::EntitySolverPyrit(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms) : EntitySolver(ID, parent, obs, ms)
{
}

EntitySolverPyrit::~EntitySolverPyrit()
{
}

void EntitySolverPyrit::createProperties(std::string& _meshFolderName, ot::UID& _meshFolderID, std::string& _meshName, ot::UID& _meshID,
										 std::string& _scriptFolderName, ot::UID& _scriptFolderID, std::string& _scriptName, ot::UID& _scriptID)
{
	EntityPropertiesSelection::createProperty("General", "Problem type", { "Custom", "Electrostatics" }, "Custom", "PyritSolver", getProperties());

	// Create the mesh property
	getProperties().createProperty(new EntityPropertiesEntityList("Mesh", _meshFolderName, _meshFolderID, _meshName, _meshID), "General");

	// Create the script property
 	getProperties().createProperty(new EntityPropertiesEntityList("Script", _scriptFolderName, _scriptFolderID, _scriptName, _scriptID), "General");

	// Create the electrostatics properties
	EntityPropertiesDouble::createProperty("Electrostatics", "Boundary potential", 0, "PyritSolver", getProperties());

	// Add a debug switch
	EntityPropertiesBoolean::createProperty("Specials", "Debug", false, "PyritSolver", getProperties());

	// Configure the visibility
	updateFromProperties();

	getProperties().forceResetUpdateForAllProperties();
}

bool EntitySolverPyrit::updateFromProperties(void)
{
	bool updatePropertiesGrid = false;

	// Now we need to update the entity after a property change
	//assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	// Check and update the visibility
	EntityPropertiesSelection* problemType = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Problem type"));

	bool boundaryPotentialVisible = problemType->getValue() == "Electrostatics";
	bool scriptVisible = problemType->getValue() == "Custom";
	bool meshVisible = problemType->getValue() != "Custom";

	EntityPropertiesEntityList* mesh = dynamic_cast<EntityPropertiesEntityList*>(getProperties().getProperty("Mesh"));
	if (meshVisible != mesh->getVisible()) updatePropertiesGrid = true;
	mesh->setVisible(meshVisible);

	EntityPropertiesDouble* boundaryPotential = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Boundary potential"));
	if (boundaryPotentialVisible != boundaryPotential->getVisible()) updatePropertiesGrid = true;
	boundaryPotential->setVisible(boundaryPotentialVisible);
	
	EntityPropertiesEntityList* customScript = dynamic_cast<EntityPropertiesEntityList*>(getProperties().getProperty("Script"));
	if (scriptVisible != customScript->getVisible()) updatePropertiesGrid = true;
	customScript->setVisible(scriptVisible);

	getProperties().forceResetUpdateForAllProperties();

	return updatePropertiesGrid;
}
