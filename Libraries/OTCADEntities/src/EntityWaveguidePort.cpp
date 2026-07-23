// @otlicense
// File: EntityWaveguidePort.cpp
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
#include "OTCADEntities/EntityWaveguidePort.h"

static EntityFactoryRegistrar<EntityWaveguidePort> registrar(EntityWaveguidePort::className());

EntityWaveguidePort::EntityWaveguidePort() : EntityFaceAnnotation(0, nullptr, nullptr, nullptr)
{
	createProperties();
}

EntityWaveguidePort::EntityWaveguidePort(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms) :
	EntityFaceAnnotation(ID, parent, obs, ms)
{
	createProperties();
}

void EntityWaveguidePort::createProperties()
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("Default/FaceAnnotationVisible");
	treeItem.setHiddenIcon("Default/FaceAnnotationHidden");
	this->setDefaultTreeItem(treeItem);

	EntityPropertiesSelection::createProperty("Port", "Port type", { "Rectangular" }, "Rectangular", "WaveguidePort", getProperties());
	EntityPropertiesSelection::createProperty("Port", "Mode", { "TE10", "TE01", "TE20", "TE21", "TE22", "TE30", "TE31", "TE32", "TE33" }, "TE10", "WaveguidePort", getProperties());
}

EntityWaveguidePort::~EntityWaveguidePort()
{
}

bool EntityWaveguidePort::updateFromProperties(void)
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	getProperties().forceResetUpdateForAllProperties();

	// The geometry will be updated as special entity update in the modeler.

	return false; // No property grid update necessary
}

void EntityWaveguidePort::addStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityFaceAnnotation::addStorageData(storage);
}

void EntityWaveguidePort::storeToDataBase(void)
{
	EntityFaceAnnotation::storeToDataBase();
}

void EntityWaveguidePort::readSpecificDataFromDataBase(const bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityFaceAnnotation::readSpecificDataFromDataBase(doc_view, entityMap);

	resetModified();
}

