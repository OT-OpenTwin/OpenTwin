// @otlicense
// File: EntityLocalCoordinateSystem.cpp
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

#include "OTModelEntities/EntityLocalCoordinateSystem.h"

#include "OTCommunication/ActionTypes.h"

#include <bsoncxx/builder/basic/array.hpp>

#include <sstream>

static EntityFactoryRegistrar<EntityLocalCoordinateSystem> registrar(EntityLocalCoordinateSystem::className());

EntityLocalCoordinateSystem::EntityLocalCoordinateSystem(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms) :
	EntityBase(ID, parent, obs, ms)
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("Default/LocalCoordinateSystemVisible");
	treeItem.setHiddenIcon("Default/LocalCoordinateSystemHidden");
	this->setDefaultTreeItem(treeItem);
}

EntityLocalCoordinateSystem::~EntityLocalCoordinateSystem()
{
}

bool EntityLocalCoordinateSystem::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

void EntityLocalCoordinateSystem::storeToDataBase(void)
{
	EntityBase::storeToDataBase();
}

void EntityLocalCoordinateSystem::addStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::addStorageData(storage);

	// Now we store the particular information about the current object
}


void EntityLocalCoordinateSystem::readSpecificDataFromDataBase(const bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	// Here we can load any special information about the material

	resetModified();
}

void EntityLocalCoordinateSystem::addVisualizationNodes(void)
{
	addVisualizationItem(getInitiallyHidden());

	EntityBase::addVisualizationNodes();
}

double EntityLocalCoordinateSystem::getValue(const std::string& groupName, const std::string& propName)
{
	EntityPropertiesDouble* property = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("#" + propName, groupName));
	assert(property != nullptr);

	if (property == nullptr) return 0.0;

	return property->getValue();
}

void EntityLocalCoordinateSystem::addVisualizationItem(bool isHidden)
{
	std::vector<double> coordinateSettings;
	coordinateSettings.reserve(9);

	coordinateSettings.push_back(getValue("Origin", "X"));
	coordinateSettings.push_back(getValue("Origin", "Y"));
	coordinateSettings.push_back(getValue("Origin", "Z"));

	coordinateSettings.push_back(getValue("z-Axis", "X"));
	coordinateSettings.push_back(getValue("z-Axis", "Y"));
	coordinateSettings.push_back(getValue("z-Axis", "Z"));

	coordinateSettings.push_back(getValue("x-Axis", "X"));
	coordinateSettings.push_back(getValue("x-Axis", "Y"));
	coordinateSettings.push_back(getValue("x-Axis", "Z"));

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddLCSNode, doc.GetAllocator()), doc.GetAllocator());

	doc.AddMember(OT_ACTION_PARAM_TreeItem, ot::JsonObject(this->getTreeItem(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_VisualizationTypes, ot::JsonObject(this->getVisualizationTypes(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_POSITION, ot::JsonArray(coordinateSettings, doc.GetAllocator()), doc.GetAllocator());

	getObserver()->sendMessageToViewer(doc);
}

void EntityLocalCoordinateSystem::updateVisualizationItem()
{
	std::vector<double> coordinateSettings;
	coordinateSettings.reserve(9);

	coordinateSettings.push_back(getValue("Origin", "X"));
	coordinateSettings.push_back(getValue("Origin", "Y"));
	coordinateSettings.push_back(getValue("Origin", "Z"));

	coordinateSettings.push_back(getValue("z-Axis", "X"));
	coordinateSettings.push_back(getValue("z-Axis", "Y"));
	coordinateSettings.push_back(getValue("z-Axis", "Z"));

	coordinateSettings.push_back(getValue("x-Axis", "X"));
	coordinateSettings.push_back(getValue("x-Axis", "Y"));
	coordinateSettings.push_back(getValue("x-Axis", "Z"));

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_UpdateLCSNode, doc.GetAllocator()), doc.GetAllocator());

	doc.AddMember(OT_ACTION_PARAM_TreeItem, ot::JsonObject(this->getTreeItem(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_POSITION, ot::JsonArray(coordinateSettings, doc.GetAllocator()), doc.GetAllocator());

	getObserver()->sendMessageToViewer(doc);
}

void EntityLocalCoordinateSystem::removeChild(EntityBase *child)
{
	EntityBase::removeChild(child);
}

void EntityLocalCoordinateSystem::createOrientationProperty(const std::string& groupName, const std::string& propName, double value)
{
	EntityPropertiesDouble* doubleProp = new EntityPropertiesDouble("#" + propName, value);
	doubleProp->setVisible(false);

	getProperties().createProperty(doubleProp, groupName);

	std::ostringstream oss;
	oss << std::defaultfloat << value;

	EntityPropertiesString::createProperty(groupName, propName, oss.str(), "", getProperties());
}

void EntityLocalCoordinateSystem::createProperties(void)
{
	createOrientationProperty("Origin", "X", 0.0);
	createOrientationProperty("Origin", "Y", 0.0);
	createOrientationProperty("Origin", "Z", 0.0);

	createOrientationProperty("z-Axis", "X", 0.0);
	createOrientationProperty("z-Axis", "Y", 0.0);
	createOrientationProperty("z-Axis", "Z", 1.0);

	createOrientationProperty("x-Axis", "X", 1.0);
	createOrientationProperty("x-Axis", "Y", 0.0);
	createOrientationProperty("x-Axis", "Z", 0.0);

	getProperties().forceResetUpdateForAllProperties();
}

bool EntityLocalCoordinateSystem::updateFromProperties(void)
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	return false; // No property grid change is necessary
}

