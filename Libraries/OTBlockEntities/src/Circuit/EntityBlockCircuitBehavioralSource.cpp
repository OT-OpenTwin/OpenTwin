// @otlicense
// File: EntityBlockCircuitBehavioralSource.cpp
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
#include "OTGui/Graphics/GraphicsItemFileCfg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTBlockEntities/Circuit/EntityBlockCircuitBehavioralSource.h"

static EntityFactoryRegistrar<EntityBlockCircuitBehavioralSource> registrar(EntityBlockCircuitBehavioralSource::className());

EntityBlockCircuitBehavioralSource::EntityBlockCircuitBehavioralSource(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms)
	: EntityBlockCircuitElement(ID, parent, obs, ms)
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("Default/VoltageSource");
	treeItem.setHiddenIcon("Default/VoltageSource");
	this->setDefaultTreeItem(treeItem);

	setBlockTitle("B");

	const std::string connectorNameLeft = "positivePole";
	m_PositiveConnector = { ot::ConnectorType::Any, connectorNameLeft, connectorNameLeft };
	addConnector(m_PositiveConnector);

	const std::string connectorNameRight = "negativePole";
	m_NegativeConnector = { ot::ConnectorType::Any, connectorNameRight, connectorNameRight };
	addConnector(m_NegativeConnector);

	resetModified();
}

void EntityBlockCircuitBehavioralSource::createProperties() {
	EntityBlockCircuitElement::createProperties();
	EntityPropertiesSelection::createProperty("Element Property", "Source Type", { "V", "I" }, "V", "default", getProperties());
	EntityPropertiesString::createProperty("Element Property", "Expression", "5", "default", getProperties());
}

double EntityBlockCircuitBehavioralSource::getRotation() const {
	auto propertyBase = getProperties().getProperty("Rotation");
	auto propertyRotation = dynamic_cast<const EntityPropertiesDouble*>(propertyBase);
	assert(propertyBase != nullptr);
	double value = propertyRotation->getValue();
	return value;
}

bool EntityBlockCircuitBehavioralSource::getFlipHorizontal() const {
	auto propertyBase = getProperties().getProperty("Flip Horizontal");
	auto propertyFlip = dynamic_cast<const EntityPropertiesBoolean*>(propertyBase);
	assert(propertyBase != nullptr);
	return propertyFlip->getValue();
}

bool EntityBlockCircuitBehavioralSource::getFlipVertical() const {
	auto propertyBase = getProperties().getProperty("Flip Vertical");
	auto propertyFlip = dynamic_cast<const EntityPropertiesBoolean*>(propertyBase);
	assert(propertyBase != nullptr);
	return propertyFlip->getValue();
}

std::string EntityBlockCircuitBehavioralSource::getSourceType() {
	auto propertyBase = getProperties().getProperty("Source Type");
	auto prop = dynamic_cast<EntityPropertiesSelection*>(propertyBase);
	if (prop) {
		return prop->getValue();
	}
	return "V";
}

std::string EntityBlockCircuitBehavioralSource::getExpression() {
	auto propertyBase = getProperties().getProperty("Expression");
	auto prop = dynamic_cast<EntityPropertiesString*>(propertyBase);
	if (prop) {
		return prop->getValue();
	}
	return "";
}

std::string EntityBlockCircuitBehavioralSource::getTypeAbbreviation() {
	return "B";
}

std::string EntityBlockCircuitBehavioralSource::getFolderName() {
	return "Behavioral Source";
}

ot::GraphicsItemCfg* EntityBlockCircuitBehavioralSource::createBlockCfg() {
	ot::GraphicsItemFileCfg* newConfig = new ot::GraphicsItemFileCfg;
	newConfig->setName("EntityBlockCircuitBehavioralSource");
	newConfig->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsMoveable | ot::GraphicsItemCfg::ItemSnapsToGridTopLeft | ot::GraphicsItemCfg::ItemUserTransformEnabled | ot::GraphicsItemCfg::ItemParticipatesInStateHandling | ot::GraphicsItemCfg::ItemForwardsState | ot::GraphicsItemCfg::ItemIsSelectable);
	newConfig->setFile("Circuit/BehavioralSource.ot.json");

	std::string name = getNameOnly();
	newConfig->addStringMapEntry("Name", name);

	double rotation = getRotation();
	bool flipH = getFlipHorizontal();
	bool flipV = getFlipVertical();

	ot::Transform transform;
	transform.setRotation(rotation);
	transform.setFlipState(ot::Transform::FlipHorizontally, flipH);
	transform.setFlipState(ot::Transform::FlipVertically, flipV);
	newConfig->setTransform(transform);

	return newConfig;
}

bool EntityBlockCircuitBehavioralSource::updateFromProperties(void)
{
	bool refresh = false;
	refresh = EntityBlockCircuitElement::updateFromProperties();

	if (refresh) {
		getProperties().forceResetUpdateForAllProperties();
	}

	return refresh;
}

void EntityBlockCircuitBehavioralSource::addStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBlock::addStorageData(storage);
}

void EntityBlockCircuitBehavioralSource::readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBlock::readSpecificDataFromDataBase(doc_view, entityMap);
}
