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
#include "OTCore/JSON/JSON.h"
#include "OTCommunication/ActionTypes.h"
#include "OTModelEntities/EntityContainer.h"

// MongoDB header
#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntityContainer> registrar(EntityContainer::className());

ot::EntityTreeItem EntityContainer::createDefaultTreeItem() {
	ot::EntityTreeItem treeItem;
	treeItem.setVisibleIcon("Default/ContainerVisible");
	treeItem.setHiddenIcon("Default/ContainerHidden");
	treeItem.setSelectChilds(true);
	return treeItem;
}

EntityContainer::EntityContainer(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms) :
	EntityBase(ID, parent, obs, ms),
	m_createVisualizationItem(true)
{
	this->setDefaultTreeItem(createDefaultTreeItem());
}

EntityContainer::~EntityContainer()
{
	std::list<EntityBase *> allChildren = m_children;  // Create a backup of the list, since the delete operation of an entity will modify the parent list

	for (EntityBase* child : allChildren)
	{
		delete child;
	}

	m_children.clear();
}

void EntityContainer::addChild(EntityBase *child)
{
	assert(std::find(m_children.begin(), m_children.end(), child) == m_children.end());  // Check that the child does not yet exist in the container

	m_children.push_back(child);

	if (!m_createVisualizationItem)
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
	if (m_createVisualizationItem == flag) return;

	m_createVisualizationItem = flag;
	
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
	assert(std::find(m_children.begin(), m_children.end(), child) != m_children.end());  // Check that the child does exist in the container

	m_children.remove(child);

	setModified();
}

bool EntityContainer::replaceChild(EntityBase* _child)
{
	EntityBase* currentEntry = nullptr;
	for (EntityBase* child : m_children)
	{
		if (child->getEntityID() == _child->getEntityID())
		{
			currentEntry = child;
			break;
		}
	}
	if (currentEntry == nullptr)
	{
		return false;
	}
	else
	{
		m_children.remove(currentEntry);
		m_children.push_back(_child);
		return true;
	}
}

void EntityContainer::takeOverChildren(EntityContainer* _other)
{
	m_children = _other->m_children;
	_other->m_children.clear();
}

const std::list<EntityBase *> &EntityContainer::getChildrenList()
{
	return m_children;
}

std::list<EntityBase*> EntityContainer::getAllChildren() const
{
	std::list<EntityBase *> allChildren;
	for (EntityBase* child : m_children)
	{
		allChildren.push_back(child);
		EntityContainer *childContainer = dynamic_cast<EntityContainer *>(child);
		if (childContainer != nullptr)
		{
			std::list<EntityBase *> childChildren = childContainer->getAllChildren();
			allChildren.splice(allChildren.end(), std::move(childChildren));
		}
	}

	return allChildren;
}

bool EntityContainer::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	xmin = xmax = ymin = ymax = zmin = zmax = 0.0;

	bool boxSet = false;

	for (EntityBase* entity : m_children)
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

		for (EntityBase* child : m_children)
		{
			EntityBase *entity = child->getEntityFromName(n);
			if (entity != nullptr) return entity;
		}
	}

	return nullptr;
}

void EntityContainer::storeToDataBase()
{
	// First, we need to store all the child items
	for (EntityBase* child : m_children)
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

	for (EntityBase* child : m_children)
	{
		childID.append((long long) child->getEntityID());
	}

	storage.append(bsoncxx::builder::basic::kvp("ChildID", childID));
	storage.append(bsoncxx::builder::basic::kvp("VisualizationItem", m_createVisualizationItem));
}

void EntityContainer::readSpecificDataFromDataBase(const bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	if (getModelState() != nullptr)	// We can only read the children, if we have the model state information (otherwise we can not determine the current version)
	{
		m_children.clear();

		// Now we read the information about the children
		try
		{
			auto childID = doc_view["ChildID"].get_array();

			for (const bsoncxx::array::element& child : childID.value)
			{
				ot::UID childID = child.get_int64();

				EntityBase *childEntity = readEntityFromEntityID(this, childID, entityMap);

				if (childEntity != nullptr)
				{
					m_children.push_back(childEntity);
				}
			}
		}
		catch (std::exception) 
		{
			m_children.clear();
		}
	}

	m_createVisualizationItem = true;
	auto docIt = doc_view.find("VisualizationItem");
	if (docIt != doc_view.end()) {
		m_createVisualizationItem = docIt->get_bool();
	}

	resetModified();
}

void EntityContainer::addVisualizationNodes() 
{
	if (!m_createVisualizationItem) return;

	if (!getName().empty())
	{
		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddContainerNode, doc.GetAllocator()), doc.GetAllocator());
		
		doc.AddMember(OT_ACTION_PARAM_TreeItem, ot::JsonObject(this->getTreeItem(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_VisualizationTypes, ot::JsonObject(this->getVisualizationTypes(), doc.GetAllocator()), doc.GetAllocator());

		getObserver()->sendMessageToViewer(doc);
	}
	
	for (EntityBase* child : m_children)
	{
		child->addVisualizationNodes();
	}

	EntityBase::addVisualizationNodes();
}

void EntityContainer::addPrefetchingRequirementsForTopology(std::list<ot::UID> &prefetchIds)
{
	EntityBase::addPrefetchingRequirementsForTopology(prefetchIds);

	for (EntityBase* child : m_children)
	{
		child->addPrefetchingRequirementsForTopology(prefetchIds);
	}
}

EntityContainer::entityType EntityContainer::getEntityType() const
{
	if (m_createVisualizationItem)
	{
		return TOPOLOGY;
	}
	
	// Now we need to check whether the item has any TOPOLOGY children

	bool anyTopologyChild = false;

	for (EntityBase* child : m_children)
	{
		if (child->getEntityType() == TOPOLOGY)
		{
			return TOPOLOGY;
		}
	}

	return DATA;
}

void EntityContainer::detachFromHierarchy()
{
	// Here we detach the entity from the hierarcy which means detaching from the parent and removing all references to the children.
	// The actual children are not deleted.

	EntityBase::detachFromHierarchy();  // Detach from parent

	// Detach children
	for (EntityBase* child : m_children)
	{
		assert(child->getParent() == this);
		child->setParent(nullptr);
	}

	m_children.clear();   // Clear child references
}

std::string EntityContainer::serialiseAsJSON()
{
	// We do not serialize top-level entities
	if (getName().find('/') == std::string::npos)
	{
		return std::string();
	}

	// Serialize general entity data
	auto docBlock = EntityBase::serialiseAsMongoDocument();
	const std::string jsonDocBlock = bsoncxx::to_json(docBlock);
	ot::JsonDocument entireDoc;
	entireDoc.fromJson(jsonDocBlock);

	// We do not serialize the children information, since the children will also be copied separately

	return entireDoc.toJson();
}

bool EntityContainer::deserialiseFromJSON(const ot::ConstJsonObject& _serialisation, const ot::CopyInformation& _copyInformation, std::map<ot::UID, EntityBase*>& _entityMap) noexcept
{
	try
	{
		// De-Serialize this entity together with the corresponding brep and facets
		const std::string serialisationString = ot::json::toJson(_serialisation);
		std::string_view serialisedEntityJSONView(serialisationString);
		auto serialisedEntityBSON = bsoncxx::from_json(serialisedEntityJSONView);
		auto serialisedEntityBSONView = serialisedEntityBSON.view();

		readSpecificDataFromDataBase(serialisedEntityBSONView, _entityMap);
		setEntityID(createEntityUID());

		_entityMap[getEntityID()] = this;

		return true;
	}
	catch (std::exception _e)
	{
		OT_LOG_E("Failed to deserialise " + getClassName() + " because: " + std::string(_e.what()));
		return false;
	}
}

void EntityContainer::fillContextMenu(const ot::MenuRequestData* _requestData, ot::MenuCfg& _menuCfg)
{
	if (getName() == "Geometry")
	{
		_menuCfg.addButton("Show Geometry Only", "Show Geometry Only", "ContextMenu/ShowGeometryOnly.png", ot::MenuButtonCfg::ButtonAction::TriggerButton)->setTriggerButton("View/Visibility/Show Geometry Only");
	}

	EntityBase::fillContextMenu(_requestData, _menuCfg);
}
