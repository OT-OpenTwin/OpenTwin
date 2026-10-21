// @otlicense
// File: EntityResultFarfield.cpp
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

// Entity.cpp : Defines the Entity class which is exported for the DLL application.
//

#include "OTCommunication/ActionTypes.h"
#include "OTModelEntities/DataBase.h"
#include "OTModelEntities/EntityResultFarfield.h"

#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntityResultFarfield> registrar("EntityResultFarfield");

EntityResultFarfield::EntityResultFarfield(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms) :
	EntityVis2D3D(ID, parent, obs, ms)
{
}

EntityResultFarfield::~EntityResultFarfield()
{
}

bool EntityResultFarfield::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

void EntityResultFarfield::storeToDataBase(void)
{
	EntityVis2D3D::storeToDataBase();
}

void EntityResultFarfield::addStorageData(bsoncxx::builder::basic::document &storage)
{
	EntityVis2D3D::addStorageData(storage);
	// Now we store the particular information about the current object
	storage.append(
		bsoncxx::builder::basic::kvp("complex", complex)
	);
}

void EntityResultFarfield::readSpecificDataFromDataBase(const bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityVis2D3D::readSpecificDataFromDataBase(doc_view, entityMap);

	// Here we can load any special information about the entity
	complex = doc_view["complex"].get_bool();

	resetModified();
}

void EntityResultFarfield::createProperties(void)
{
	EntityPropertiesSelection::createProperty("General", "Frequency", frequencies, frequencies.empty() ? "" : frequencies.front(), "", getProperties());
	EntityPropertiesSelection::createProperty("General", "Type", {"Magnitude", "Phase", "Real", "Imaginary"}, "Magnitude", "", getProperties());

	EntityPropertiesDouble::createProperty("Phi", "Phi min (deg.)", phiMin, "", getProperties())->setReadOnly(true);
	EntityPropertiesDouble::createProperty("Phi", "Phi max (deg.)", phiMax, "", getProperties())->setReadOnly(true);
	EntityPropertiesInteger::createProperty("Phi", "Phi steps", phiSteps, "", getProperties())->setReadOnly(true);

	EntityPropertiesDouble::createProperty("Theta", "Theta min (deg.)", thetaMin, "", getProperties())->setReadOnly(true);
	EntityPropertiesDouble::createProperty("Theta", "Theta max (deg.)", thetaMax, "", getProperties())->setReadOnly(true);
	EntityPropertiesInteger::createProperty("Theta", "Theta steps", thetaSteps, "", getProperties())->setReadOnly(true);

	getProperties().forceResetUpdateForAllProperties();

	updatePropertyVisibilities();
}

bool EntityResultFarfield::updatePropertyVisibilities(void)
{
	bool updatePropertiesGrid = false;

	EntityPropertiesSelection* typeProperty = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Type"));
	if (typeProperty != nullptr)
	{
		if (typeProperty->getVisible() != complex)
		{
			typeProperty->setVisible(complex);
			updatePropertiesGrid = true;
		}
	}

	return updatePropertiesGrid;
}

bool EntityResultFarfield::updateFromProperties(void)
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	getProperties().forceResetUpdateForAllProperties();

	return false; // Notify, whether property grid update is necessary
}

