// @otlicense
// File: EntityParameter.cpp
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

#include "DataBase.h"
#include "EntityParameter.h"

#include "OTCommunication/ActionTypes.h"

#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntityParameter> registrar("EntityParameter");

EntityParameter::EntityParameter(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms) :
	EntityBase(ID, parent, obs, ms)
{
	ot::EntityTreeItem treeItem;
	treeItem.setVisibleIcon("Default/ParameterVisible");
	treeItem.setHiddenIcon("Default/ParameterHidden");
	this->setTreeItem(treeItem, true);
}

EntityParameter::~EntityParameter()
{
}

bool EntityParameter::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

void EntityParameter::addStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::addStorageData(storage);

	// Add the dependency information to the storage

	auto arrayUID      = bsoncxx::builder::basic::array();
	auto arrayProperty = bsoncxx::builder::basic::array();

	for (auto &entityDependency : dependencyMap)
	{
		for (auto &propertyDependency : entityDependency.second)
		{
			arrayUID.append((long long) entityDependency.first);
			arrayProperty.append(propertyDependency.first);
		}
	}

	storage.append(
		bsoncxx::builder::basic::kvp("dependUID",  arrayUID),
		bsoncxx::builder::basic::kvp("dependProp", arrayProperty)
	);
}

void EntityParameter::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now load the dependency information from the storage
	dependencyMap.clear();

	try
	{
		auto arrayUID      = doc_view["dependUID"].get_array().value;
		auto arrayProperty = doc_view["dependProp"].get_array().value;

		size_t nDependencies = std::distance(arrayUID.begin(), arrayUID.end());
		assert(nDependencies == std::distance(arrayProperty.begin(), arrayProperty.end()));

		auto iUID = arrayUID.begin();
		auto iProperty = arrayProperty.begin();

		for (unsigned long index = 0; index < nDependencies; index++)
		{
			const auto& bStringValue = iProperty->get_utf8().value;
			addDependencyByIndex(iUID->get_int64(), std::string(bStringValue.data()));

			iUID++;
			iProperty++;
		}
	}
	catch (std::exception)
	{
	}

	resetModified();
}

void EntityParameter::addDependencyByIndex(ot::UID entityID, const std::string& propertyIndex)
{
	if (dependencyMap.count(entityID) > 0)
	{

		if (dependencyMap[entityID].count(propertyIndex) > 0)
		{
			// This dependency already exists -> nothing to change
			return;
		}
	}

	dependencyMap[entityID][propertyIndex] = true;
	setModified();
}

void EntityParameter::addVisualizationNodes(void)
{
	addVisualizationItem(getInitiallyHidden());

	EntityBase::addVisualizationNodes();
}

void EntityParameter::addVisualizationItem(bool isHidden)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddContainerNode, doc.GetAllocator()), doc.GetAllocator());

	doc.AddMember(OT_ACTION_PARAM_TreeItem, ot::JsonObject(this->getTreeItem(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_VisualizationTypes, ot::JsonObject(this->getVisualizationTypes(), doc.GetAllocator()), doc.GetAllocator());

	getObserver()->sendMessageToViewer(doc);
}

void EntityParameter::createProperties(void)
{
	EntityPropertiesString::createProperty("General", "Value", "0", "", getProperties());
	EntityPropertiesString::createProperty("General", "Description", "", "", getProperties());

	EntityPropertiesDouble *doubleProp = new EntityPropertiesDouble("#Value", 0.0);
	doubleProp->setVisible(false);

	getProperties().createProperty(doubleProp, "");

	getProperties().forceResetUpdateForAllProperties();
}

void EntityParameter::setValue(const std::string &value)
{
	EntityPropertiesString *stringProperty = dynamic_cast<EntityPropertiesString *>(getProperties().getProperty("Value"));

	if (stringProperty != nullptr)
	{
		stringProperty->setValue(value);
	}
	else
	{
		assert(0);
	}
}

std::string EntityParameter::getValue(void)
{
	EntityPropertiesString *stringProperty = dynamic_cast<EntityPropertiesString *>(getProperties().getProperty("Value"));

	if (stringProperty != nullptr)
	{
		return stringProperty->getValue();
	}
	
	assert(0);
	return "";
}

double EntityParameter::getNumericValue(void)
{
	EntityPropertiesDouble *doubleProperty = dynamic_cast<EntityPropertiesDouble *>(getProperties().getProperty("#Value"));

	if (doubleProperty != nullptr)
	{
		return doubleProperty->getValue();
	}

	assert(0);
	return 0.0;
}

bool EntityParameter::updateFromProperties(void)
{
	setModified();
	getProperties().forceResetUpdateForAllProperties();  // We don't need to do anything special here. The necessary updates with be done in the calling function

	return false;
}

void EntityParameter::addDependency(ot::UID entityID, const std::string &propertyName, const std::string& propertyGroup)
{
	const std::string key =	EntityProperties::createKey(propertyName, propertyGroup);
	addDependencyByIndex(entityID, key);
}

void EntityParameter::removeDependency(ot::UID entityID, const std::string &propertyName, const std::string& propertyGroup)
{
	if (dependencyMap.count(entityID) > 0)
	{
		const std::string key = EntityProperties::createKey(propertyName, propertyGroup);

		if (dependencyMap[entityID].count(key) > 0)
		{
			dependencyMap[entityID].erase(key);

			if (dependencyMap[entityID].empty())
			{
				dependencyMap.erase(entityID);
			}

			setModified();
		}
	}
}

void EntityParameter::removeDependency(ot::UID entityID)
{
	if (dependencyMap.count(entityID) > 0)
	{
		dependencyMap.erase(entityID);
		setModified();
	}
}

bool EntityParameter::hasDependency(ot::UID entityID)
{
	return (dependencyMap.count(entityID) > 0);
}

void EntityParameter::removeAllDependencies(void)
{
	dependencyMap.clear();
	setModified();
}

