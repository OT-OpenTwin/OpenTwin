// @otlicense
// File: EntityBlockCircuitSubcircuit.cpp
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
#include "OTBlockEntities/Circuit/EntityBlockCircuitSubcircuit.h"
#include "OTGui/Graphics/GraphicsItemFileCfg.h"
#include "OTModelEntities/Properties/EntityPropertiesItems.h"

// Third party header
#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntityBlockCircuitSubcircuit> registrar(EntityBlockCircuitSubcircuit::className());

EntityBlockCircuitSubcircuit::EntityBlockCircuitSubcircuit(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms)
	: EntityBlockCircuitElement(ID, parent, obs, ms)
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("CircuitElementImages/Subcircuit");
	treeItem.setHiddenIcon("CircuitElementImages/Subcircuit");
	this->setDefaultTreeItem(treeItem);
	setBlockTitle("SUBCKT");

	registerCallbacks(
		ot::EntityCallbackBase::Callback::DataHandle,
		OT_INFO_SERVICE_TYPE_CircuitSimulatorService
	);

	resetModified();
}

void EntityBlockCircuitSubcircuit::createProperties()
{
	EntityBlockCircuitElement::createProperties();
	EntityPropertiesString::createProperty("Subcircuit Properties", "Model Name", "Subcircuit1", "default", getProperties());
}


bool EntityBlockCircuitSubcircuit::updateFromProperties(void)
{
	bool refresh = EntityBlockCircuitElement::updateFromProperties();
	auto prop = getProperties().getProperty("Model Name");
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


std::string EntityBlockCircuitSubcircuit::getSubcircuitModelName() const
{
	auto prop = getProperties().getProperty("Model Name");
	auto strProp = dynamic_cast<const EntityPropertiesString*>(prop);
	if (strProp)
	{
		return strProp->getValue();
	}
	return getBlockTitle();
}

void EntityBlockCircuitSubcircuit::setSubcircuitModelName(const std::string& _name)
{
	auto prop = getProperties().getProperty("Model Name");
	auto strProp = dynamic_cast<EntityPropertiesString*>(prop);
	if (strProp)
	{
		strProp->setValue(_name);
	}
	setBlockTitle(_name);
	setModified();
}

double EntityBlockCircuitSubcircuit::getRotation() const
{
	auto propertyBase = getProperties().getProperty("Rotation");
	auto propertyRotation = dynamic_cast<const EntityPropertiesDouble*>(propertyBase);
	return propertyRotation ? propertyRotation->getValue() : 0.0;
}
bool EntityBlockCircuitSubcircuit::getFlipHorizontal() const
{
	auto propertyBase = getProperties().getProperty("Flip Horizontal");
	auto propertyFlip = dynamic_cast<const EntityPropertiesBoolean*>(propertyBase);
	return propertyFlip ? propertyFlip->getValue() : false;
}
bool EntityBlockCircuitSubcircuit::getFlipVertical() const
{
	auto propertyBase = getProperties().getProperty("Flip Vertical");
	auto propertyFlip = dynamic_cast<const EntityPropertiesBoolean*>(propertyBase);
	return propertyFlip ? propertyFlip->getValue() : false;
}
void EntityBlockCircuitSubcircuit::syncConnectors(const std::vector<std::string>& _orderedTerminalNames)
{
	m_orderedTerminalNames = _orderedTerminalNames;
	// Update connectors
	for (const auto& terminalName : m_orderedTerminalNames)
	{
		ot::Connector connector = { ot::ConnectorType::Any, terminalName, terminalName };
		this->addConnector(connector);
	}
	setModified();
}


ot::GraphicsItemCfg* EntityBlockCircuitSubcircuit::createBlockCfg()
{
	ot::GraphicsItemFileCfg* newConfig = new ot::GraphicsItemFileCfg;
	newConfig->setName("EntityBlockCircuitSubcircuit");
	newConfig->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsMoveable | ot::GraphicsItemCfg::ItemSnapsToGridTopLeft | ot::GraphicsItemCfg::ItemUserTransformEnabled | ot::GraphicsItemCfg::ItemParticipatesInStateHandling | ot::GraphicsItemCfg::ItemForwardsState | ot::GraphicsItemCfg::ItemIsSelectable | ot::GraphicsItemCfg::ItemIsDoubleClickable);
	newConfig->setFile("Circuit/Subcircuit.ot.json");
	newConfig->addStringMapEntry("Name", getSubcircuitModelName());
	ot::Transform transform;
	transform.setRotation(getRotation());
	transform.setFlipState(ot::Transform::FlipHorizontally, getFlipHorizontal());
	transform.setFlipState(ot::Transform::FlipVertically, getFlipVertical());
	newConfig->setTransform(transform);
	return newConfig;
}


void EntityBlockCircuitSubcircuit::addStorageData(bsoncxx::builder::basic::document& _storage)
{
	EntityBlock::addStorageData(_storage);
	bsoncxx::builder::basic::array terminalArray;
	for (const auto& terminal : m_orderedTerminalNames)
	{
		terminalArray.append(terminal);
	}
	_storage.append(bsoncxx::builder::basic::kvp("OrderedTerminals", terminalArray));
}

void EntityBlockCircuitSubcircuit::readSpecificDataFromDataBase(const bsoncxx::document::view& _doc_view, std::map<ot::UID, EntityBase*>& _entityMap)
{
	EntityBlock::readSpecificDataFromDataBase(_doc_view, _entityMap);
	m_orderedTerminalNames.clear();
	if (_doc_view.find("OrderedTerminals") != _doc_view.end())
	{
		for (const auto& terminalElem : _doc_view["OrderedTerminals"].get_array().value)
		{
			m_orderedTerminalNames.push_back(std::string(terminalElem.get_string().value));
		}
	}
	else if (_doc_view.find("OrderedPorts") != _doc_view.end())
	{
		for (const auto& portElem : _doc_view["OrderedPorts"].get_array().value)
		{
			m_orderedTerminalNames.push_back(std::string(portElem.get_string().value));
		}
	}
}