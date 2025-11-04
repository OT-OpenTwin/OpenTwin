// @otlicense
// File: EntityBlockCircuitTransmissionLine.cpp
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

#include "EntityBlockCircuitTransmissionLine.h"
#include "OTCommunication/ActionTypes.h"
#include "OTGui/GraphicsStackItemCfg.h"
#include "OTGui/GraphicsImageItemCfg.h"
#include "OTGui/GraphicsHBoxLayoutItemCfg.h"
#include "OTGui/GraphicsRectangularItemCfg.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/GraphicsGridLayoutItemCfg.h"
#include "OTGui/GraphicsEllipseItemCfg.h"
#include "OTGui/GraphicsItemFileCfg.h"

static EntityFactoryRegistrar<EntityBlockCircuitTransmissionLine> registrar(EntityBlockCircuitTransmissionLine::className());

EntityBlockCircuitTransmissionLine::EntityBlockCircuitTransmissionLine(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner)
	:EntityBlockCircuitElement(ID, parent, obs, ms, owner)
{
	OldTreeIcon icon;
	icon.visibleIcon = "TranLine";
	icon.hiddenIcon = "TranLine";
	setNavigationTreeIcon(icon);

	setBlockTitle("T");

	const std::string connectorNameLeftPos1 = "PositivePole1";
	m_LeftConnectorPos1 = { ot::ConnectorType::Out,connectorNameLeftPos1,connectorNameLeftPos1 };
	addConnector(m_LeftConnectorPos1);

	const std::string connectorNameRightNeg1 = "NegativePole1";
	m_RightConnectorNeg1 = { ot::ConnectorType::In,connectorNameRightNeg1,connectorNameRightNeg1 };
	addConnector(m_RightConnectorNeg1);

	const std::string connectorNameLeftPos2 = "PositivePole2";
	m_LeftConnectorPos2 = { ot::ConnectorType::Out,connectorNameLeftPos2,connectorNameLeftPos2 };
	addConnector(m_LeftConnectorPos2);

	const std::string connectorNameRightNeg2 = "NegativePole2";
	m_RightConnectorNeg2 = { ot::ConnectorType::In,connectorNameRightNeg2,connectorNameRightNeg2 };
	addConnector(m_RightConnectorNeg2);

	resetModified();
}

void EntityBlockCircuitTransmissionLine::createProperties() {
	EntityBlockCircuitElement::createProperties();
	EntityPropertiesString::createProperty("Element Property", "Characteristic Impedance", "50", "default", getProperties());
	EntityPropertiesString::createProperty("Element Property", "Transmission Delay", "10NS", "default", getProperties());
}

std::string EntityBlockCircuitTransmissionLine::getImpedance() {
	auto propertyBase = getProperties().getProperty("Characteristic Impedance");
	auto elementType = dynamic_cast<EntityPropertiesString*>(propertyBase);
	assert(elementType != nullptr);

	return elementType->getValue();
}

std::string EntityBlockCircuitTransmissionLine::getTransmissionDelay() {
	auto propertyBase = getProperties().getProperty("Transmission Delay");
	auto elementType = dynamic_cast<EntityPropertiesString*>(propertyBase);
	assert(elementType != nullptr);

	return elementType->getValue();
}

std::string EntityBlockCircuitTransmissionLine::getTypeAbbreviation() {
	return "T";
}

std::string EntityBlockCircuitTransmissionLine::getFolderName() {
	return "Transmission Line";
}

ot::GraphicsItemCfg* EntityBlockCircuitTransmissionLine::createBlockCfg() {
	ot::GraphicsItemFileCfg* newConfig = new ot::GraphicsItemFileCfg;
	newConfig->setName("EntityBlockCircuitTransmissionLine");
	newConfig->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsMoveable | ot::GraphicsItemCfg::ItemSnapsToGridTopLeft | ot::GraphicsItemCfg::ItemUserTransformEnabled | ot::GraphicsItemCfg::ItemForwardsState | ot::GraphicsItemCfg::ItemIsSelectable);
	newConfig->setFile("Circuit/TransmissionLine.ot.json");

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

double EntityBlockCircuitTransmissionLine::getRotation() {
	auto propertyBase = getProperties().getProperty("Rotation");
	auto propertyRotation = dynamic_cast<EntityPropertiesDouble*>(propertyBase);
	assert(propertyBase != nullptr);
	double value = propertyRotation->getValue();
	return value;
}

std::string EntityBlockCircuitTransmissionLine::getFlip() {
	auto propertyBase = getProperties().getProperty("Flip");
	auto propertyFlip = dynamic_cast<EntityPropertiesSelection*>(propertyBase);
	assert(propertyBase != nullptr);
	std::string value = propertyFlip->getValue();
	return value;
}

bool EntityBlockCircuitTransmissionLine::updateFromProperties(void) {
	bool refresh = false;
	refresh = EntityBlockCircuitElement::updateFromProperties();

	if (refresh) {
		getProperties().forceResetUpdateForAllProperties();

	}

	return refresh;
}



void EntityBlockCircuitTransmissionLine::addStorageData(bsoncxx::builder::basic::document& storage) {
	EntityBlock::addStorageData(storage);
}

void EntityBlockCircuitTransmissionLine::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) {
	EntityBlock::readSpecificDataFromDataBase(doc_view, entityMap);
}

