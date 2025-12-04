// @otlicense
// File: EntityVisUnstructuredScalarVolume.cpp
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


#include "EntityVisUnstructuredScalarVolume.h"
#include "OldTreeIcon.h"
#include "Database.h"

#include "OTCommunication/ActionTypes.h"

#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntityVisUnstructuredScalarVolume> registrar("EntityVisUnstructuredScalarVolume");

EntityVisUnstructuredScalarVolume::EntityVisUnstructuredScalarVolume(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms) :
	EntityVis2D3D(ID, parent, obs, ms)
{
}

EntityVisUnstructuredScalarVolume::~EntityVisUnstructuredScalarVolume()
{

}

void EntityVisUnstructuredScalarVolume::addStorageData(bsoncxx::builder::basic::document& storage)
{
	// We store the parent class information first 
	EntityVis2D3D::addStorageData(storage);

}

void EntityVisUnstructuredScalarVolume::readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	// We read the parent class information first 
	EntityVis2D3D::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now load the data from the storage

	resetModified();
}

void EntityVisUnstructuredScalarVolume::createProperties(void)
{
	assert(resultType != EntityResultBase::UNKNOWN);

	propertyBundleVisUnstructuredScalar.setProperties(this);
	propertyBundlePlane.setProperties(this);
	propertyBundleScaling.setProperties(this);

	updatePropertyVisibilities();

	getProperties().forceResetUpdateForAllProperties();
}

bool EntityVisUnstructuredScalarVolume::updatePropertyVisibilities(void)
{
	bool updatePropertiesGrid = false;
	bool is2dType = false;

	updatePropertiesGrid |= propertyBundleVisUnstructuredScalar.updatePropertyVisibility(this);

	if (propertyBundleVisUnstructuredScalar.is2dType(this))
	{
		updatePropertiesGrid |= propertyBundlePlane.updatePropertyVisibility(this);
	}
	else
	{
		updatePropertiesGrid |= propertyBundlePlane.hidePlane(this);
	}

	updatePropertiesGrid |= propertyBundleScaling.updatePropertyVisibility(this);

	return updatePropertiesGrid;
}


bool EntityVisUnstructuredScalarVolume::updateFromProperties(void)
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	// Here we need to update the plot (send a message to the visualization service)
	getProperties().forceResetUpdateForAllProperties();

	return updatePropertyVisibilities(); // No property grid update necessary
}
