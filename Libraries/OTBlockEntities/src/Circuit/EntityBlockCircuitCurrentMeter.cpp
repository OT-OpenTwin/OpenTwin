// @otlicense
// File: EntityBlockCircuitCurrentMeter.cpp
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
#include "OTGui/Graphics/GraphicsItemFileCfg.h"
#include "OTBlockEntities/Circuit/EntityBlockCircuitCurrentMeter.h"

static EntityFactoryRegistrar<EntityBlockCircuitCurrentMeter> registrar(EntityBlockCircuitCurrentMeter::className());

EntityBlockCircuitCurrentMeter::EntityBlockCircuitCurrentMeter(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms)
	:EntityBlockCircuitElement(ID, parent, obs, ms)
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("Default/CurrentMeter");
	treeItem.setHiddenIcon("Default/CurrentMeter");
	this->setDefaultTreeItem(treeItem);
	
	setBlockTitle("CM");

	const std::string connectorNameLeft = "positivePole";
	m_LeftConnector = { ot::ConnectorType::Any,connectorNameLeft,connectorNameLeft };
	addConnector(m_LeftConnector);

	const std::string connectorNameRight = "negativePole";
	m_RightConnector = { ot::ConnectorType::Any,connectorNameRight,connectorNameRight };
	addConnector(m_RightConnector);

	resetModified();
}

void EntityBlockCircuitCurrentMeter::createProperties() {
	EntityBlockCircuitElement::createProperties();
	EntityPropertiesDouble::createProperty("Transform-Properties", "Rotation", 0.0, "default", getProperties());
	EntityPropertiesSelection::createProperty("Transform-Properties", "Flip", { "NoFlip" , "FlipVertically" , "FlipHorizontally" }, "NoFlip", "default", getProperties());
}

std::string EntityBlockCircuitCurrentMeter::getTypeAbbreviation() {
	return "CM";
}

std::string EntityBlockCircuitCurrentMeter::getFolderName() {
	return "Current Meter";
}

const double EntityBlockCircuitCurrentMeter::getRotation() const {
	auto propertyBase = getProperties().getProperty("Rotation");
	auto propertyRotation = dynamic_cast<const EntityPropertiesDouble*>(propertyBase);
	assert(propertyBase != nullptr);
	double value = propertyRotation->getValue();
	return value;
}

 const std::string EntityBlockCircuitCurrentMeter::getFlip() const {
	auto propertyBase = getProperties().getProperty("Flip");
	auto propertyFlip = dynamic_cast<const EntityPropertiesSelection*>(propertyBase);
	assert(propertyBase != nullptr);
	std::string value = propertyFlip->getValue();
	return value;
}

ot::GraphicsItemCfg* EntityBlockCircuitCurrentMeter::createBlockCfg() {
	ot::GraphicsItemFileCfg* newConfig = new ot::GraphicsItemFileCfg;
	newConfig->setName("EntityBlockCircuitInductor");
	newConfig->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsMoveable | ot::GraphicsItemCfg::ItemSnapsToGridTopLeft | ot::GraphicsItemCfg::ItemUserTransformEnabled | ot::GraphicsItemCfg::ItemForwardsState | ot::GraphicsItemCfg::ItemIsSelectable);
	newConfig->setFile("Circuit/AmpereMeter.ot.json");
	
	std::string name = getNameOnly();
	newConfig->addStringMapEntry("Name", name);

	//Map of String to Enum
	std::map<std::string, ot::Transform::FlipState> stringFlipMap;
	stringFlipMap.insert_or_assign("NoFlip", ot::Transform::NoFlip);
	stringFlipMap.insert_or_assign("FlipVertically", ot::Transform::FlipVertically);
	stringFlipMap.insert_or_assign("FlipHorizontally", ot::Transform::FlipHorizontally);



	double rotation = getRotation();
	std::string flip = getFlip();
	ot::Transform::FlipState flipState(stringFlipMap[flip]);


	ot::Transform transform;
	transform.setRotation(rotation);
	transform.setFlipState(flipState);
	newConfig->setTransform(transform);

	return newConfig;
}

bool EntityBlockCircuitCurrentMeter::updateFromProperties(void) {

	bool refresh = false;
	refresh = EntityBlockCircuitElement::updateFromProperties();

	if (refresh) {
		getProperties().forceResetUpdateForAllProperties();

	}

	return refresh;
}

void EntityBlockCircuitCurrentMeter::addStorageData(bsoncxx::builder::basic::document& storage) {
	EntityBlock::addStorageData(storage);
}

void EntityBlockCircuitCurrentMeter::readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) {
	EntityBlock::readSpecificDataFromDataBase(doc_view, entityMap);
}