// @otlicense
// File: EntityFieldDump.cpp
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

// OpenTwin header
#include "OTCommunication/ActionTypes.h"
#include "OTModelEntities/DataBase.h"
#include "OTModelEntities/EntityFacetData.h"
#include "OTCADEntities/EntityBrep.h"
#include "OTCADEntities/EntityGeometry.h"
#include "OTCADEntities/GeometryOperations.h"
#include "OTCADEntities/EntityFieldDump.h"

static EntityFactoryRegistrar<EntityFieldDump> registrar(EntityFieldDump::className());

EntityFieldDump::EntityFieldDump() : EntityFaceAnnotation(0, nullptr, nullptr, nullptr)
{
	createProperties();
}

EntityFieldDump::EntityFieldDump(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms) :
	EntityFaceAnnotation(ID, parent, obs, ms)
{
	createProperties();
}

void EntityFieldDump::createProperties()
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("Default/FieldDumpVisible");
	treeItem.setHiddenIcon("Default/FieldDumpHidden");
	this->setDefaultTreeItem(treeItem);

	getProperties().getProperty("Color")->setVisible(false);
	getProperties().getProperty("Number of faces")->setVisible(false);

	EntityPropertiesSelection::createProperty("General", "Type", {
											  "Electric Field (Time Domain)",
											  "Magnetic Field (Time Domain)",
											  "Electric Current Density (Time Domain)",
											  "Total Current Density (Time Domain)",
											  "Electric Field (Frequency Domain)",
											  "Magnetic Field (Frequency Domain)",
											  "Electric Current Density (Frequency Domain)",
											  "Total Current Density (Frequency Domain)",
											  "Local SAR",
											  "1 g Averaged SAR",
											  "10 g Averaged SAR",
											  "SAR Raw Data"
											  }, "Electric Field (Frequency Domain)", "FieldDump", getProperties());
	//EntityPropertiesSelection::createProperty("Port", "Mode", { "TE10", "TE01", "TE20", "TE21", "TE22", "TE30", "TE31", "TE32", "TE33" }, "TE10", "WaveguidePort", getProperties());
}

EntityFieldDump::~EntityFieldDump()
{
}

bool EntityFieldDump::updateFromProperties(void)
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	getProperties().forceResetUpdateForAllProperties();

	// The geometry will be updated as special entity update in the modeler.

	return false; // No property grid update necessary
}

void EntityFieldDump::addStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityFaceAnnotation::addStorageData(storage);
}

void EntityFieldDump::storeToDataBase(void)
{
	EntityFaceAnnotation::storeToDataBase();
}

void EntityFieldDump::readSpecificDataFromDataBase(const bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityFaceAnnotation::readSpecificDataFromDataBase(doc_view, entityMap);

	resetModified();
}

