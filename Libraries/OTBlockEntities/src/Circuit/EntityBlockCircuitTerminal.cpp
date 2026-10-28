// @otlicense
// File: EntityBlockCircuitTerminal.cpp
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
#include "OTBlockEntities/Circuit/EntityBlockCircuitTerminal.h"
#include "OTGui/Graphics/GraphicsItemFileCfg.h"
#include "OTModelEntities/Properties/EntityPropertiesItems.h"
#include "OTCommunication/ActionTypes.h"

static EntityFactoryRegistrar<EntityBlockCircuitTerminal> registrar(EntityBlockCircuitTerminal::className());

EntityBlockCircuitTerminal::EntityBlockCircuitTerminal(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms)
	: EntityBlockCircuitElement(ID, parent, obs, ms)
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("CircuitElementImages/Terminal");
	treeItem.setHiddenIcon("CircuitElementImages/Terminal");
	this->setDefaultTreeItem(treeItem);
	setBlockTitle("Terminal");
	
	// Exact one connector for terminals. The name of the connector is "pin" and it can be connected to any other connector.
	const std::string connectorName = "pin";
	m_Connector = { ot::ConnectorType::Any, connectorName, connectorName };
	addConnector(m_Connector);
	resetModified();
}

void EntityBlockCircuitTerminal::createProperties()
{
	EntityBlockCircuitElement::createProperties();

	// Name of the terminal which connects outside the subcircuit.
	EntityPropertiesString::createProperty("Terminal Properties", "Terminal Name", "IN", "default", getProperties());

	// Side of the terminal. This is used to determine the position of the terminal in the subcircuit block.
	EntityPropertiesSelection::createProperty("Terminal Properties", "Side", { "Left", "Right", "Top", "Bottom" }, "Left", "default", getProperties());

	// Add initial suffix to the name if it does not already have one
	std::string currentName = getName();
	if (!currentName.empty() && currentName.rfind(" (") == std::string::npos)
	{
		setName(currentName + " (" + getTerminalName() + ")");
	}
}

bool EntityBlockCircuitTerminal::updateFromProperties(void)
{
	bool refresh = false;

	auto prop = getProperties().getProperty("Terminal Name");
	if (prop && prop->needsUpdate())
	{
		createBlockItem();
		refresh = true;

		// 1. Get basic path without suffix
		std::string currentPath = getName();
		std::string basePath = currentPath;
		size_t pos = basePath.rfind(" (");
		if (pos != std::string::npos && basePath.back() == ')')
		{
			basePath = basePath.substr(0, pos);
		}

		// 2. Merge new Path with suffix
		auto strProp = dynamic_cast<EntityPropertiesString*>(prop);
		std::string newName = "";
		if (strProp)
		{
			newName = strProp->getValue();
		}
		setBlockTitle(newName);

		std::string newPath = basePath + " (" + newName + ")";

		// 3. Update name if it has changed
		if (newPath != currentPath)
		{
			setName(newPath);

			if (getObserver() != nullptr)
			{
				ot::JsonDocument doc;
				doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_RenameEntityName, doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_PATH_FROM, ot::JsonString(currentPath, doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_PATH_To, ot::JsonString(newPath, doc.GetAllocator()), doc.GetAllocator());

				getObserver()->sendMessageToViewer(doc);
			}
		}
	}

	auto sideProp = getProperties().getProperty("Side");
	if (sideProp && sideProp->needsUpdate())
	{
		refresh = true;
		setModified();
		if (getObserver() != nullptr)
		{
			getObserver()->connectionChanged(this);
		}
	}


	refresh = EntityBlockCircuitElement::updateFromProperties() || refresh;

	if (refresh)
	{
		getProperties().forceResetUpdateForAllProperties();
	}
	return refresh;
}

std::string EntityBlockCircuitTerminal::getTerminalName() const
{
	auto prop = getProperties().getProperty("Terminal Name");
	auto strProp = dynamic_cast<const EntityPropertiesString*>(prop);
	if (strProp)
	{
		return strProp->getValue();
	}
	std::string name = getNameOnly();
	size_t pos = name.rfind(" (");
	if (pos != std::string::npos && name.back() == ')')
	{
		return name.substr(pos + 2, name.size() - pos - 3);
	}
	return getBlockTitle();
}

void EntityBlockCircuitTerminal::setTerminalName(const std::string& _name)
{
	auto prop = getProperties().getProperty("Terminal Name");
	auto strProp = dynamic_cast<EntityPropertiesString*>(prop);
	if (strProp)
	{
		strProp->setValue(_name);
	}
	setBlockTitle(_name);

	std::string currentPath = getName();
	if (!currentPath.empty())
	{
		std::string basePath = currentPath;
		size_t pos = basePath.rfind(" (");
		if (pos != std::string::npos && basePath.back() == ')')
		{
			basePath = basePath.substr(0, pos);
		}
		std::string newPath = basePath + " (" + _name + ")";
		if (newPath != currentPath)
		{
			setName(newPath);
			if (getObserver() != nullptr)
			{
				ot::JsonDocument doc;
				doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_RenameEntityName, doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_PATH_FROM, ot::JsonString(currentPath, doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_PATH_To, ot::JsonString(newPath, doc.GetAllocator()), doc.GetAllocator());

				getObserver()->sendMessageToViewer(doc);
			}
		}
	}

	if (getObserver() != nullptr)
	{
		createBlockItem();
	}

	setModified();
}

std::string EntityBlockCircuitTerminal::getTerminalSide() const
{
	auto prop = getProperties().getProperty("Side");
	auto selProp = dynamic_cast<const EntityPropertiesSelection*>(prop);
	if (selProp)
	{
		return selProp->getValue();
	}
	return "Left";
}

void EntityBlockCircuitTerminal::setTerminalSide(const std::string& _side)
{
	auto prop = getProperties().getProperty("Side");
	auto selProp = dynamic_cast<EntityPropertiesSelection*>(prop);
	if (selProp)
	{
		selProp->setValue(_side);
		setModified();
	}
}

double EntityBlockCircuitTerminal::getRotation() const
{
	auto propertyBase = getProperties().getProperty("Rotation");
	auto propertyRotation = dynamic_cast<const EntityPropertiesDouble*>(propertyBase);
	return propertyRotation ? propertyRotation->getValue() : 0.0;
}

bool EntityBlockCircuitTerminal::getFlipHorizontal() const
{
	auto propertyBase = getProperties().getProperty("Flip Horizontal");
	auto propertyFlip = dynamic_cast<const EntityPropertiesBoolean*>(propertyBase);
	return propertyFlip ? propertyFlip->getValue() : false;
}

bool EntityBlockCircuitTerminal::getFlipVertical() const
{
	auto propertyBase = getProperties().getProperty("Flip Vertical");
	auto propertyFlip = dynamic_cast<const EntityPropertiesBoolean*>(propertyBase);
	return propertyFlip ? propertyFlip->getValue() : false;
}

ot::GraphicsItemCfg* EntityBlockCircuitTerminal::createBlockCfg()
{
	ot::GraphicsItemFileCfg* newConfig = new ot::GraphicsItemFileCfg;
	newConfig->setName("EntityBlockCircuitTerminal");
	newConfig->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsMoveable | ot::GraphicsItemCfg::ItemSnapsToGridTopLeft | ot::GraphicsItemCfg::ItemUserTransformEnabled | ot::GraphicsItemCfg::ItemParticipatesInStateHandling | ot::GraphicsItemCfg::ItemForwardsState | ot::GraphicsItemCfg::ItemIsSelectable);
	newConfig->setFile("Circuit/Terminal.ot.json");
	newConfig->addStringMapEntry("Name", getTerminalName());
	ot::Transform transform;
	transform.setRotation(getRotation());
	transform.setFlipState(ot::Transform::FlipHorizontally, getFlipHorizontal());
	transform.setFlipState(ot::Transform::FlipVertically, getFlipVertical());
	newConfig->setTransform(transform);
	return newConfig;
}

void EntityBlockCircuitTerminal::addStorageData(bsoncxx::builder::basic::document& _storage)
{
	EntityBlock::addStorageData(_storage);
}

void EntityBlockCircuitTerminal::readSpecificDataFromDataBase(const bsoncxx::document::view& _doc_view, std::map<ot::UID, EntityBase*>& _entityMap)
{
	EntityBlock::readSpecificDataFromDataBase(_doc_view, _entityMap);
}
