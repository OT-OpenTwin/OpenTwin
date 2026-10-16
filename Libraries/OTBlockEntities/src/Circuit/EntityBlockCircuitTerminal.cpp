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

static EntityFactoryRegistrar<EntityBlockCircuitTerminal> registrar(EntityBlockCircuitTerminal::className());

EntityBlockCircuitTerminal::EntityBlockCircuitTerminal(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms)
	: EntityBlockCircuitElement(ID, parent, obs, ms)
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("CircuitElementImages/Terminal");
	treeItem.setHiddenIcon("CircuitElementImages/Terminal");
	this->setDefaultTreeItem(treeItem);
	setBlockTitle("TERMINAL");
	
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
	// Optional index 
	EntityPropertiesInteger::createProperty("Terminal Properties", "Terminal Index", 1, "default", getProperties());
}

bool EntityBlockCircuitTerminal::updateFromProperties(void)
{
	bool refresh = EntityBlockCircuitElement::updateFromProperties();
	auto prop = getProperties().getProperty("Terminal Name");
	if (prop && prop->needsUpdate())
	{
		auto strProp = dynamic_cast<EntityPropertiesString*>(prop);
		if (strProp)
		{
			setBlockTitle(strProp->getValue());
		}
		refresh = true;
	}
	if (refresh)
	{
		getProperties().forceResetUpdateForAllProperties();
	}
	return refresh;
}

std::string EntityBlockCircuitTerminal::getTerminalName() const
{
	auto prop = getProperties().getProperty("Terminal Name");
	if (!prop)
	{
		return getBlockTitle();
	}
	auto strProp = dynamic_cast<const EntityPropertiesString*>(prop);
	if (strProp)
	{
		return strProp->getValue();
	}
	return getBlockTitle();
}

int EntityBlockCircuitTerminal::getTerminalIndex() const
{
	auto prop = getProperties().getProperty("Terminal Index");
	if (!prop)
	{
		return 1;
	}
	auto intProp = dynamic_cast<const EntityPropertiesInteger*>(prop);
	if (intProp)
	{
		return intProp->getValue();
	}
	return 1;
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
