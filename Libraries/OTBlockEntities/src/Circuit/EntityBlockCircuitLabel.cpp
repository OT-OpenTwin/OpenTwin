// @otlicense
// File: EntityBlockCircuitLabel.cpp
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
#include "OTBlockEntities/Circuit/EntityBlockCircuitLabel.h"

static EntityFactoryRegistrar<EntityBlockCircuitLabel> registrar(EntityBlockCircuitLabel::className());

EntityBlockCircuitLabel::EntityBlockCircuitLabel(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms)
	: EntityBlockCircuitElement(ID, parent, obs, ms)
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("Default/GND");
	treeItem.setHiddenIcon("Default/GND");
	this->setDefaultTreeItem(treeItem);

	setBlockTitle("Label");

	const std::string connectorNameLeft = "flagPole";
	m_LeftConnector = { ot::ConnectorType::Any,connectorNameLeft,connectorNameLeft };
	addConnector(m_LeftConnector);

	resetModified();
}

void EntityBlockCircuitLabel::createProperties() {
	EntityPropertiesString::createProperty("Label-Properties", "Label Name", "MyLabel", "default", getProperties());
	EntityPropertiesDouble::createProperty("Transform-Properties", "Rotation", 0.0, "default", getProperties());
	EntityPropertiesBoolean::createProperty("Transform-Properties", "Flip Horizontal", false, "default", getProperties());
	EntityPropertiesBoolean::createProperty("Transform-Properties", "Flip Vertical", false, "default", getProperties());
}

std::string EntityBlockCircuitLabel::getTypeAbbreviation() {
	return "LBL";
}

std::string EntityBlockCircuitLabel::getFolderName() {
	return "Label";
}

std::string EntityBlockCircuitLabel::getLabelName() const {
	auto propertyBase = getProperties().getProperty("Label Name");
	auto propertyName = dynamic_cast<const EntityPropertiesString*>(propertyBase);
	if (propertyName) {
		return propertyName->getValue();
	}
	return getNameOnly();
}

double EntityBlockCircuitLabel::getRotation() const {
	auto propertyBase = getProperties().getProperty("Rotation");
	auto propertyRotation = dynamic_cast<const EntityPropertiesDouble*>(propertyBase);
	assert(propertyBase != nullptr);
	double value = propertyRotation->getValue();
	return value;
}

bool EntityBlockCircuitLabel::getFlipHorizontal() const {
	auto propertyBase = getProperties().getProperty("Flip Horizontal");
	auto propertyFlip = dynamic_cast<const EntityPropertiesBoolean*>(propertyBase);
	assert(propertyBase != nullptr);
	return propertyFlip->getValue();
}

bool EntityBlockCircuitLabel::getFlipVertical() const {
	auto propertyBase = getProperties().getProperty("Flip Vertical");
	auto propertyFlip = dynamic_cast<const EntityPropertiesBoolean*>(propertyBase);
	assert(propertyBase != nullptr);
	return propertyFlip->getValue();
}

ot::GraphicsItemCfg* EntityBlockCircuitLabel::createBlockCfg() {
	ot::GraphicsItemFileCfg* newConfig = new ot::GraphicsItemFileCfg;
	newConfig->setName("EntityBlockCircuitLabel");
	newConfig->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsMoveable | ot::GraphicsItemCfg::ItemSnapsToGridTopLeft | ot::GraphicsItemCfg::ItemUserTransformEnabled | ot::GraphicsItemCfg::ItemParticipatesInStateHandling | ot::GraphicsItemCfg::ItemForwardsState | ot::GraphicsItemCfg::ItemIsSelectable);
	newConfig->setFile("Circuit/Label.ot.json");
	
	std::string name = getLabelName();
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

bool EntityBlockCircuitLabel::updateFromProperties(void)
{
	bool refresh = false;
	
	auto labelNameProp = getProperties().getProperty("Label Name");
	if (labelNameProp && labelNameProp->needsUpdate()) {
		createBlockItem();
		refresh = true;
	}

	refresh = EntityBlockCircuitElement::updateFromProperties() || refresh;

	return refresh;
}

void EntityBlockCircuitLabel::addStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBlock::addStorageData(storage);
}

void EntityBlockCircuitLabel::readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBlock::readSpecificDataFromDataBase(doc_view, entityMap);
}
