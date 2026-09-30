// @otlicense
// File: EntityFarfieldDump.cpp
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
#include "OTCADEntities/EntityFarfieldDump.h"

static EntityFactoryRegistrar<EntityFarfieldDump> registrar(EntityFarfieldDump::className());

EntityFarfieldDump::EntityFarfieldDump() : EntityFaceAnnotation(0, nullptr, nullptr, nullptr)
{
	createProperties();
}

EntityFarfieldDump::EntityFarfieldDump(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms) :
	EntityFaceAnnotation(ID, parent, obs, ms)
{
	createProperties();
}

void EntityFarfieldDump::createProperties()
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("Default/FieldDumpVisible");
	treeItem.setHiddenIcon("Default/FieldDumpHidden");
	this->setDefaultTreeItem(treeItem);

	getProperties().getProperty("Color")->setVisible(false);
	getProperties().getProperty("Number of faces")->setVisible(false);

	EntityPropertiesSelection::createProperty("General", "Type", { "2D", "1D (Phi constant)", "1D (Theta constant)" }, "2D", "FarfieldDump", getProperties());

	EntityPropertiesString::createProperty("General", "Frequencies", "", "FarfieldDump", getProperties())->setToolTip("Comma separated list of dump frequencies.\nExample: 10, 12");

	EntityPropertiesDouble::createProperty("Phi", "Phi cut (deg.)", 0.0, "FieldDump", getProperties())->setGroupChanges(true);
	EntityPropertiesDouble::createProperty("Phi", "Phi min (deg.)", 0.0, "FieldDump", getProperties())->setGroupChanges(true);
	EntityPropertiesDouble::createProperty("Phi", "Phi max (deg.)", 360.0, "FieldDump", getProperties())->setGroupChanges(true);
	EntityPropertiesInteger::createProperty("Phi", "Phi steps", 20, "FieldDump", getProperties())->setGroupChanges(true);

	EntityPropertiesDouble::createProperty("Theta", "Theta cut (deg.)", 90.0, "FieldDump", getProperties())->setGroupChanges(true);
	EntityPropertiesDouble::createProperty("Theta", "Theta min (deg.)", 0.0, "FieldDump", getProperties())->setGroupChanges(true);
	EntityPropertiesDouble::createProperty("Theta", "Theta max (deg.)", 180.0, "FieldDump", getProperties())->setGroupChanges(true);
	EntityPropertiesInteger::createProperty("Theta", "Theta steps", 10, "FieldDump", getProperties())->setGroupChanges(true);

	updatePropertyVisibilities();
}

EntityFarfieldDump::~EntityFarfieldDump()
{
}

bool EntityFarfieldDump::updateFromProperties(void)
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	bool visibilityUpdated = updatePropertyVisibilities();

	getProperties().forceResetUpdateForAllProperties();

	return visibilityUpdated;
}

bool EntityFarfieldDump::updatePropertyVisibilities()
{
	EntityPropertiesSelection* typeProperty = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Type"));

	assert(typeProperty != nullptr);

	if (typeProperty == nullptr) return false;

	bool visibilityUpdated = false;

	bool phiRangeVisible = (typeProperty->getValue() == "2D" || typeProperty->getValue() == "1D (Theta constant)");
	bool thetaRangeVisible = (typeProperty->getValue() == "2D" || typeProperty->getValue() == "1D (Phi constant)");

	EntityPropertiesDouble::createProperty("Phi", "Phi cut (deg.)", 0.0, "FieldDump", getProperties())->setGroupChanges(true);
	EntityPropertiesDouble::createProperty("Phi", "Phi min (deg.)", 0.0, "FieldDump", getProperties())->setGroupChanges(true);
	EntityPropertiesDouble::createProperty("Phi", "Phi max (deg.)", 360.0, "FieldDump", getProperties())->setGroupChanges(true);
	EntityPropertiesInteger::createProperty("Phi", "Phi steps", 20, "FieldDump", getProperties())->setGroupChanges(true);

	EntityPropertiesDouble::createProperty("Theta", "Theta cut (deg.)", 90.0, "FieldDump", getProperties())->setGroupChanges(true);
	EntityPropertiesDouble::createProperty("Theta", "Theta min (deg.)", 0.0, "FieldDump", getProperties())->setGroupChanges(true);
	EntityPropertiesDouble::createProperty("Theta", "Theta max (deg.)", 180.0, "FieldDump", getProperties())->setGroupChanges(true);
	EntityPropertiesInteger::createProperty("Theta", "Theta steps", 10, "FieldDump", getProperties())->setGroupChanges(true);

	visibilityUpdated |= updatePropertyVisibility("Phi cut (deg.)", !phiRangeVisible);
	visibilityUpdated |= updatePropertyVisibility("Phi min (deg.)", phiRangeVisible);
	visibilityUpdated |= updatePropertyVisibility("Phi max (deg.)", phiRangeVisible);
	visibilityUpdated |= updatePropertyVisibility("Phi steps", phiRangeVisible);

	visibilityUpdated |= updatePropertyVisibility("Theta cut (deg.)", !thetaRangeVisible);
	visibilityUpdated |= updatePropertyVisibility("Theta min (deg.)", thetaRangeVisible);
	visibilityUpdated |= updatePropertyVisibility("Theta max (deg.)", thetaRangeVisible);
	visibilityUpdated |= updatePropertyVisibility("Theta steps", thetaRangeVisible);

	return visibilityUpdated;
}

bool EntityFarfieldDump::updatePropertyVisibility(const std::string& propertyName, bool visible)
{
	auto property = getProperties().getProperty(propertyName);
	if (property == nullptr)
	{
		assert(0); // Property not found
		return false;
	}

	if (property->getVisible() == visible) return false;

	property->setVisible(visible);
	return true;
}

void EntityFarfieldDump::addStorageData(bsoncxx::builder::basic::document& storage)
{
	// We store the parent class information first 
	EntityFaceAnnotation::addStorageData(storage);
}

void EntityFarfieldDump::storeToDataBase(void)
{
	EntityFaceAnnotation::storeToDataBase();
}

void EntityFarfieldDump::readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	// We read the parent class information first 
	EntityFaceAnnotation::readSpecificDataFromDataBase(doc_view, entityMap);

	resetModified();
}

