// @otlicense
// File: EntityContainer.cpp
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
#include "EntityContainer.h"
#include "OldTreeIcon.h"
#include "OTCore/JSON.h"
#include <OTCommunication/ActionTypes.h>

// MongoDB header
#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntityContainer> registrar(EntityContainer::className());

EntityContainer::EntityContainer(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner) :
	EntityBase(ID, parent, obs, ms, owner),
	createVisualizationItem(true)
{
	m_treeIcon.size = 32;
	m_treeIcon.visibleIcon = "ContainerVisible";
	m_treeIcon.hiddenIcon = "ContainerHidden";
}

EntityContainer::~EntityContainer()
{
	std::list<EntityBase *> allChildren = children;  // Create a backup of the list, since the delete operation of an entity will modify the parent list

	for (auto child : allChildren)
	{
		delete child;
	}

	children.clear();
}

void EntityContainer::addChild(EntityBase *child)
{
	assert(std::find(children.begin(), children.end(), child) == children.end());  // Check that the child does not yet exist in the container

	children.push_back(child);

	if (!createVisualizationItem)
	{
		// Let's check whether a child needs the visualitation item. If so, turn it on and also inform our parents
		if (child->getEntityType() != DATA)
		{
			if (dynamic_cast<EntityContainer *> (child) != nullptr) 
			{
				// Our child is a container, too. We need to check its flag
				if (dynamic_cast<EntityContainer *> (child)->getCreateVisualizationItem())
				{
					setCreateVisualizationItem(true);
				}
			}
			else
			{
				setCreateVisualizationItem(true);
			}
		}
	}

	setModified();
}

void EntityContainer::setCreateVisualizationItem(bool flag)
{
	if (createVisualizationItem == flag) return;

	createVisualizationItem = flag;
	
	if (flag)
	{
		EntityContainer *parentContainer = dynamic_cast<EntityContainer *> (getParent());
		if (parentContainer != nullptr)
		{
			parentContainer->setCreateVisualizationItem(true);
		}
	}
}

void EntityContainer::removeChild(EntityBase *child)
{
	assert(std::find(children.begin(), children.end(), child) != children.end());  // Check that the child does exist in the container

	children.remove(child);

	setModified();
}

const std::list<EntityBase *> &EntityContainer::getChildrenList(void)
{
	return children;
}

bool EntityContainer::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	xmin = xmax = ymin = ymax = zmin = zmax = 0.0;

	bool boxSet = false;

	for (auto entity : children)
	{
		double exmin(0.0), exmax(0.0), eymin(0.0), eymax(0.0), ezmin(0.0), ezmax(0.0);

		if (entity->getEntityBox(exmin, exmax, eymin, eymax, ezmin, ezmax))
		{
			if (!boxSet)
			{
				boxSet = true;

				xmin = exmin;
				xmax = exmax;
				ymin = eymin;
				ymax = eymax;
				zmin = ezmin;
				zmax = ezmax;
			}
			else
			{
				xmin = std::min(xmin, exmin);
				xmax = std::max(xmax, exmax);
				ymin = std::min(ymin, eymin);
				ymax = std::max(ymax, eymax);
				zmin = std::min(zmin, ezmin);
				zmax = std::max(zmax, ezmax);
			}
		}
	}

	return boxSet;
}

EntityBase *EntityContainer::getEntityFromName(const std::string &n)
{
	if (getName().size() > n.size()) return nullptr; // Our name is too long, so it can't be us or one of our children

	if (getName() == n) return this;  // It's us.

	if (n.substr(0, getName().size()) == getName())
	{
		// At least the first part of the name matches, so it could be one of our children

		for (auto child : children)
		{
			EntityBase *entity = child->getEntityFromName(n);
			if (entity != nullptr) return entity;
		}
	}

	return nullptr;
}

void EntityContainer::storeToDataBase(void)
{
	// First, we need to store all the child items
	for (auto child : children)
	{
		child->storeToDataBase();
	}

	// Afterward, we store the container itself
	EntityBase::storeToDataBase();
}

void EntityContainer::addStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::addStorageData(storage);

	// Now we store the particular information about the current object (the children)

	auto childID = bsoncxx::builder::basic::array{};

	for (auto child : children)
	{
		childID.append((long long) child->getEntityID());
	}

	storage.append(bsoncxx::builder::basic::kvp("ChildID", childID));
	storage.append(bsoncxx::builder::basic::kvp("VisualizationItem", createVisualizationItem));
}

void EntityContainer::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	if (getModelState() != nullptr)	// We can only read the children, if we have the model state information (otherwise we can not determine the current version)
	{
		children.clear();

		// Now we read the information about the children
		try
		{
			auto childID = doc_view["ChildID"].get_array();

			for (bsoncxx::array::element child : childID.value)
			{
				ot::UID childID = child.get_int64();

				EntityBase *childEntity = readEntityFromEntityID(this, childID, entityMap);

				if (childEntity != nullptr)
				{
					children.push_back(childEntity);
				}
			}
		}
		catch (std::exception) 
		{
			children.clear();
		}
	}

	createVisualizationItem = true;
	try
	{
		createVisualizationItem = doc_view["VisualizationItem"].get_bool();
	}
	catch (std::exception) 
	{
	}

	resetModified();
}

void EntityContainer::addVisualizationNodes(void) 
{
	if (!createVisualizationItem) return;

	if (!getName().empty())
	{
		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddContainerNode, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_UI_TREE_Name, ot::JsonString(this->getName(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getEntityID(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsEditable, this->getEditable(), doc.GetAllocator());

		m_treeIcon.addToJsonDoc(doc);

		getObserver()->sendMessageToViewer(doc);
	}
	
	for (EntityBase* child : children)
	{
		child->addVisualizationNodes();
	}

	EntityBase::addVisualizationNodes();
}

void EntityContainer::addPrefetchingRequirementsForTopology(std::list<ot::UID> &prefetchIds)
{
	EntityBase::addPrefetchingRequirementsForTopology(prefetchIds);

	for (auto child : children)
	{
		child->addPrefetchingRequirementsForTopology(prefetchIds);
	}
}

EntityContainer::entityType EntityContainer::getEntityType(void) const
{
	if (createVisualizationItem)
	{
		return TOPOLOGY;
	}
	
	// Now we need to check whether the item has any TOPOLOGY children

	bool anyTopologyChild = false;

	for (auto child : children)
	{
		if (child->getEntityType() == TOPOLOGY)
		{
			return TOPOLOGY;
		}
	}

	return DATA;
}

void EntityContainer::detachFromHierarchy(void)
{
	// Here we detach the entity from the hierarcy which means detaching from the parent and removing all references to the children.
	// The actual children are not deleted.

	EntityBase::detachFromHierarchy();  // Detach from parent

	// Detach children
	for (auto child : children)
	{
		assert(child->getParent() == this);
		child->setParent(nullptr);
	}

	children.clear();   // Clear child references
}
