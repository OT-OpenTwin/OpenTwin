// @otlicense
// File: EntityVisUnstructuredScalarSurface.cpp
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

#include "Database.h"
#include "EntityVisUnstructuredScalarSurface.h"

#include "OTCommunication/ActionTypes.h"

#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntityVisUnstructuredScalarSurface> registrar("EntityVisUnstructuredScalarSurface");

EntityVisUnstructuredScalarSurface::EntityVisUnstructuredScalarSurface(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms) :
	EntityVis2D3D(ID, parent, obs, ms)
{
}

EntityVisUnstructuredScalarSurface::~EntityVisUnstructuredScalarSurface()
{

}

void EntityVisUnstructuredScalarSurface::addStorageData(bsoncxx::builder::basic::document& storage)
{
	// We store the parent class information first 
	EntityVis2D3D::addStorageData(storage);

}

void EntityVisUnstructuredScalarSurface::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	// We read the parent class information first 
	EntityVis2D3D::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now load the data from the storage

	resetModified();
}

void EntityVisUnstructuredScalarSurface::createProperties(void)
{
	assert(resultType != EntityResultBase::UNKNOWN);

	propertyBundleVisUnstructuredScalarSurface.setProperties(this);
	propertyBundleScaling.setProperties(this);

	updatePropertyVisibilities();

	getProperties().forceResetUpdateForAllProperties();
}

bool EntityVisUnstructuredScalarSurface::updatePropertyVisibilities(void)
{
	bool updatePropertiesGrid = false;
	bool is2dType = false;

	updatePropertiesGrid |= propertyBundleVisUnstructuredScalarSurface.updatePropertyVisibility(this);
	updatePropertiesGrid |= propertyBundleScaling.updatePropertyVisibility(this);

	return updatePropertiesGrid;
}


bool EntityVisUnstructuredScalarSurface::updateFromProperties(void)
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	// Here we need to update the plot (send a message to the visualization service)
	getProperties().forceResetUpdateForAllProperties();

	return updatePropertyVisibilities(); // No property grid update necessary
}
