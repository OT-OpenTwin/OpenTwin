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
											  "Electric Field (Frequency Domain)",
											  "Magnetic Field (Frequency Domain)",
											  "Electric Current Density (Frequency Domain)",
											  "Total Current Density (Frequency Domain)",
											  "Electric Field (Time Domain)",
											  "Magnetic Field (Time Domain)",
											  "Electric Current Density (Time Domain)",
											  "Total Current Density (Time Domain)",
											  "Local SAR (Frequency Domain)",
											  "1 g Averaged SAR (Frequency Domain)",
											  "10 g Averaged SAR (Frequency Domain)",
											  "SAR Raw Data (Frequency Domain)"
											  }, "Electric Field (Frequency Domain)", "FieldDump", getProperties());
	
	EntityPropertiesString::createProperty("General", "Frequencies", "", "FieldDump", getProperties())->setToolTip("Comma separated list of dump frequencies.\nExample: 10, 12");

	EntityPropertiesBoolean::createProperty("Range", "Subrange", false, "FieldDump", getProperties());
	EntityPropertiesDouble::createProperty("Range", "Xmin", 0.0, "FieldDump", getProperties());
	EntityPropertiesDouble::createProperty("Range", "Xmax", 0.0, "FieldDump", getProperties());
	EntityPropertiesDouble::createProperty("Range", "Ymin", 0.0, "FieldDump", getProperties());
	EntityPropertiesDouble::createProperty("Range", "Ymax", 0.0, "FieldDump", getProperties());
	EntityPropertiesDouble::createProperty("Range", "Zmin", 0.0, "FieldDump", getProperties());
	EntityPropertiesDouble::createProperty("Range", "Zmax", 0.0, "FieldDump", getProperties());

	EntityPropertiesSelection::createProperty("Subsampling", "Mode", { "None", "Index based", "Resolution based" }, "None", "FieldDump", getProperties());
	EntityPropertiesInteger::createProperty("Subsampling", "Step x", 1, "FieldDump", getProperties())->setToolTip("Specify index based stepwidth for recording.\nExamples: 1 means full resolution, 2 means every second sample");
	EntityPropertiesInteger::createProperty("Subsampling", "Step y", 1, "FieldDump", getProperties())->setToolTip("Specify index based stepwidth for recording.\nExamples: 1 means full resolution, 2 means every second sample");
	EntityPropertiesInteger::createProperty("Subsampling", "Step z", 1, "FieldDump", getProperties())->setToolTip("Specify index based stepwidth for recording.\nExamples: 1 means full resolution, 2 means every second sample");
	EntityPropertiesDouble::createProperty("Subsampling", "Resolution x", 0.0, "FieldDump", getProperties())->setToolTip("Specify spatial resolution for recording.");
	EntityPropertiesDouble::createProperty("Subsampling", "Resolution y", 0.0, "FieldDump", getProperties())->setToolTip("Specify spatial resolution for recording.");
	EntityPropertiesDouble::createProperty("Subsampling", "Resolution z", 0.0, "FieldDump", getProperties())->setToolTip("Specify spatial resolution for recording.");

	updatePropertyVisibilities();
}

EntityFieldDump::~EntityFieldDump()
{
}

bool EntityFieldDump::updateFromProperties(void)
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	bool visibilityUpdated = updatePropertyVisibilities();

	getProperties().forceResetUpdateForAllProperties();

	return visibilityUpdated; 
}

bool EntityFieldDump::updatePropertyVisibilities()
{
	EntityPropertiesSelection* typeProperty = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Type"));
	EntityPropertiesBoolean* subrangeProperty = dynamic_cast<EntityPropertiesBoolean*>(getProperties().getProperty("Subrange"));
	EntityPropertiesSelection* subsamplingProperty = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Mode"));

	assert(typeProperty != nullptr);
	assert(subrangeProperty != nullptr);
	assert(subsamplingProperty != nullptr);

	if (typeProperty == nullptr || subrangeProperty == nullptr || subsamplingProperty == nullptr) return false;

	bool visibilityUpdated = false;

	bool showFrequencyList = typeProperty->getValue().find("(Frequency Domain)") != std::string::npos;

	visibilityUpdated |= updatePropertyVisibility("Frequencies", showFrequencyList);

	bool subrangeVisible = subrangeProperty->getValue();

	visibilityUpdated |= updatePropertyVisibility("Xmin", subrangeVisible);
	visibilityUpdated |= updatePropertyVisibility("Xmax", subrangeVisible);
	visibilityUpdated |= updatePropertyVisibility("Ymin", subrangeVisible);
	visibilityUpdated |= updatePropertyVisibility("Ymax", subrangeVisible);
	visibilityUpdated |= updatePropertyVisibility("Zmin", subrangeVisible);
	visibilityUpdated |= updatePropertyVisibility("Zmax", subrangeVisible);

	bool subsamplingIndexVisible = (subsamplingProperty->getValue() == "Index based");
	bool subsamplingResolutionVisible = (subsamplingProperty->getValue() == "Resolution based");

	visibilityUpdated |= updatePropertyVisibility("Step x", subsamplingIndexVisible);
	visibilityUpdated |= updatePropertyVisibility("Step y", subsamplingIndexVisible);
	visibilityUpdated |= updatePropertyVisibility("Step z", subsamplingIndexVisible);

	visibilityUpdated |= updatePropertyVisibility("Resolution x", subsamplingResolutionVisible);
	visibilityUpdated |= updatePropertyVisibility("Resolution y", subsamplingResolutionVisible);
	visibilityUpdated |= updatePropertyVisibility("Resolution z", subsamplingResolutionVisible);

	return visibilityUpdated;
}

bool EntityFieldDump::updatePropertyVisibility(const std::string &propertyName, bool visible)
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

