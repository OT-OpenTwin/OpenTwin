// @otlicense
// File: EntitySolverFITTD.cpp
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


#include "EntitySolverFITTD.h"
#include "OldTreeIcon.h"

#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntitySolverFITTD> registrar("EntitySolverFITTD");

EntitySolverFITTD::EntitySolverFITTD(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner) :
	EntitySolver(ID, parent, obs, ms, owner)
{
}

EntitySolverFITTD::~EntitySolverFITTD()
{
}

void EntitySolverFITTD::createProperties(const std::string meshFolderName, ot::UID meshFolderID, const std::string meshName, ot::UID meshID)
{
	
	EntityPropertiesBoolean::createProperty("Specials", GetPropertyNameDebug(), false, "FITTD", getProperties());
	EntityPropertiesEntityList::createProperty("Mesh", GetPropertyNameMesh(), meshFolderName, meshFolderID, meshName, meshID, "FITTD", getProperties());
	EntityPropertiesInteger::createProperty("Simulation settings", GetPropertyNameTimesteps(), 1000, "FITTD", getProperties());
	EntityPropertiesSelection::createProperty("Simulation settings", GetPropertyNameDimension(), {GetPropertyValueDimension3D()}, GetPropertyValueDimension3D(), "FITTD", getProperties());
	EntityPropertiesSelection::createProperty("Volume Selection", GetPropertyNameVolumeSelection(), { GetPropertyValueVolumeFull() }, GetPropertyValueVolumeFull(), "FITTD", getProperties());
}

bool EntitySolverFITTD::updateFromProperties(void)
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	// Here we need to update the plot (send a message to the visualization service)
	getProperties().forceResetUpdateForAllProperties();

	return false;
}

const std::string EntitySolverFITTD::GetPropertyNameDebug()
{
	static const std::string propertyNameDebug = "Debug";
	return propertyNameDebug;
}

const std::string EntitySolverFITTD::GetPropertyNameMesh()
{
	static const std::string propertyNameMesh = "Mesh";
	return propertyNameMesh;
}

const std::string EntitySolverFITTD::GetPropertyNameTimesteps()
{
	static const std::string propertyNameTimesteps = "Timesteps";
	return propertyNameTimesteps;
}

const std::string EntitySolverFITTD::GetPropertyNameDimension()
{
	static const std::string propertyNameDimension = "Dimension";
	return propertyNameDimension;
}

const std::string EntitySolverFITTD::GetPropertyNameVolumeSelection()
{
	static const std::string propertyNameVolumeSelection = "Simulated volume";
	return propertyNameVolumeSelection;
}

const std::string EntitySolverFITTD::GetPropertyValueDimension3D()
{
	static const std::string propertyValueDimension3D = "3D";
	return propertyValueDimension3D;
}

const std::string EntitySolverFITTD::GetPropertyValueVolumeFull()
{
	static const std::string propertyValueVolumenFull = "Full";
	return propertyValueVolumenFull;
}
