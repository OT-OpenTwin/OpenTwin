// @otlicense
// File: EntitySolverOpenEMS.cpp
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


#include "OTModelEntities/EntitySolverOpenEMS.h"

#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntitySolverOpenEMS> registrar("EntitySolverOpenEMS");

EntitySolverOpenEMS::EntitySolverOpenEMS(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms) : EntitySolver(ID, parent, obs, ms)
{
}

EntitySolverOpenEMS::~EntitySolverOpenEMS()
{
}

void EntitySolverOpenEMS::createProperties(std::string& _meshFolderName, ot::UID& _meshFolderID, std::string& _meshName, ot::UID& _meshID)
{
	getProperties().createProperty(new EntityPropertiesEntityList("Mesh", _meshFolderName, _meshFolderID, _meshName, _meshID), "General");

	// Boundary conditions value list
	std::list<std::string> boundaryValues{ "PEC", "PMC", "MUR", "PML_8" };
	// Excitation types lists
	std::list<std::string> excitationTypes{ "Gauss Excitation" };
	
	//EntityPropertiesDouble::createProperty("Frequency", "Start Frequency", 0, "OpenEMSSolver", getProperties());
	//EntityPropertiesDouble::createProperty("Frequency", "End Frequency", 0, "OpenEMSSolver", getProperties());
	//EntityPropertiesInteger::createProperty("Simulation Settings", "Timesteps", 10000, "OpenEMSSolver", getProperties());
	//EntityPropertiesSelection::createProperty("Simulation Settings", "Excitation Type", excitationTypes, "Gauss Excitation", "OpenEMSSolver", getProperties());
	//EntityPropertiesDouble::createProperty("Simulation Settings", "End Criteria", 1e-5, "OpenEMSSolver", getProperties());
	//EntityPropertiesInteger::createProperty("Simulation Settings", "Oversampling", 6, "OpenEMSSolver", getProperties());
	//EntityPropertiesSelection::createProperty("Boundary Conditions", "Xmin", boundaryValues, "PEC", "OpenEMSSolver", getProperties());
	//EntityPropertiesSelection::createProperty("Boundary Conditions", "Xmax", boundaryValues, "PEC", "OpenEMSSolver", getProperties());
	//EntityPropertiesSelection::createProperty("Boundary Conditions", "Ymin", boundaryValues, "PEC", "OpenEMSSolver", getProperties());
	//EntityPropertiesSelection::createProperty("Boundary Conditions", "Ymax", boundaryValues, "PEC", "OpenEMSSolver", getProperties());
	//EntityPropertiesSelection::createProperty("Boundary Conditions", "Zmin", boundaryValues, "PEC", "OpenEMSSolver", getProperties());
	//EntityPropertiesSelection::createProperty("Boundary Conditions", "Zmax", boundaryValues, "PEC", "OpenEMSSolver", getProperties());
	EntityPropertiesBoolean::createProperty("Specials", "Debug", false, "OpenEMSSolver", getProperties());


	// Configure the visibility
	updateFromProperties();

	getProperties().forceResetUpdateForAllProperties();
}

bool EntitySolverOpenEMS::updateFromProperties(void)
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

void EntitySolverOpenEMS::fillContextMenu(const ot::MenuRequestData* _requestData, ot::MenuCfg& _menuCfg)
{
	_menuCfg.addButton("Update", "Update", "ContextMenu/Run.png", ot::MenuButtonCfg::ButtonAction::TriggerButton)->setTriggerButton("OpenEMS/Solver/Run Solver");

	_menuCfg.addSeparator();

	EntitySolver::fillContextMenu(_requestData, _menuCfg);
}
