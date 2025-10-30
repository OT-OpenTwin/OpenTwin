// @otlicense
// File: EntityBlockCircuitVoltageMeter.cpp
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

#include "EntityBlockCircuitVoltageMeter.h"
#include "OTCommunication/ActionTypes.h"
#include "OTGui/GraphicsStackItemCfg.h"
#include "OTGui/GraphicsImageItemCfg.h"
#include "OTGui/GraphicsHBoxLayoutItemCfg.h"
#include "OTGui/GraphicsRectangularItemCfg.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/GraphicsGridLayoutItemCfg.h"
#include "OTGui/GraphicsEllipseItemCfg.h"
#include "OTGui/GraphicsItemFileCfg.h"

static EntityFactoryRegistrar<EntityBlockCircuitVoltageMeter> registrar(EntityBlockCircuitVoltageMeter::className());

EntityBlockCircuitVoltageMeter::EntityBlockCircuitVoltageMeter(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner)
	:EntityBlockCircuitElement(ID, parent, obs, ms, owner)
{
	OldTreeIcon icon;
	icon.visibleIcon = "VoltMeter";
	icon.hiddenIcon = "VoltMeter";
	setNavigationTreeIcon(icon);

	setBlockTitle("Voltage Meter");

	const std::string connectorNameLeft = "Left4";
	m_LeftConnector = { ot::ConnectorType::Out,connectorNameLeft,connectorNameLeft };
	addConnector(m_LeftConnector);

	const std::string connectorNameRight = "Right4";
	m_RightConnector = { ot::ConnectorType::In,connectorNameRight,connectorNameRight };
	addConnector(m_RightConnector);

	resetModified();
}

void EntityBlockCircuitVoltageMeter::createProperties(const std::string& _circuitModelFolderName, const ot::UID& _circuitModelFolderID) {
	EntityBlockCircuitElement::createProperties(_circuitModelFolderName,_circuitModelFolderID);
	EntityPropertiesDouble::createProperty("Transform-Properties", "Rotation", 0.0, "default", getProperties());
	EntityPropertiesSelection::createProperty("Transform-Properties", "Flip", { "NoFlip" , "FlipVertically" , "FlipHorizontally" }, "NoFlip", "default", getProperties());
}

std::string EntityBlockCircuitVoltageMeter::getTypeAbbreviation() {
	return "VM";
}

std::string EntityBlockCircuitVoltageMeter::getFolderName() {
	return "Voltage Meter";
}

double EntityBlockCircuitVoltageMeter::getRotation() {
	auto propertyBase = getProperties().getProperty("Rotation");
	auto propertyRotation = dynamic_cast<EntityPropertiesDouble*>(propertyBase);
	assert(propertyBase != nullptr);
	double value = propertyRotation->getValue();
	return value;
}

std::string EntityBlockCircuitVoltageMeter::getFlip() {
	auto propertyBase = getProperties().getProperty("Flip");
	auto propertyFlip = dynamic_cast<EntityPropertiesSelection*>(propertyBase);
	assert(propertyBase != nullptr);
	std::string value = propertyFlip->getValue();
	return value;
}

#define TEST_ITEM_LOADER true
ot::GraphicsItemCfg* EntityBlockCircuitVoltageMeter::createBlockCfg() {
#if TEST_ITEM_LOADER==true
	ot::GraphicsItemFileCfg* newConfig = new ot::GraphicsItemFileCfg;
	newConfig->setName("EntityBlockCircuitVoltageMeter");
	newConfig->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsMoveable | ot::GraphicsItemCfg::ItemSnapsToGridTopLeft | ot::GraphicsItemCfg::ItemUserTransformEnabled | ot::GraphicsItemCfg::ItemForwardsState | ot::GraphicsItemCfg::ItemIsSelectable);
	newConfig->setFile("Circuit/VoltageMeter.ot.json");

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


#endif
	ot::GraphicsStackItemCfg* myStack = new ot::GraphicsStackItemCfg();
	myStack->setName("EntityBlock");
	myStack->setTitle("EntityBlockCircuitVoltageMeter");
	myStack->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsMoveable);

	//Second I create an Image
	ot::GraphicsImageItemCfg* image = new ot::GraphicsImageItemCfg();
	image->setImagePath("CircuitElementImages/VoltageMeter.png");
	image->setSizePolicy(ot::SizePolicy::Dynamic);
	image->setMaintainAspectRatio(true);

	myStack->addItemBottom(image, false, true);

	//Then I create a layout
	ot::GraphicsHBoxLayoutItemCfg* myLayout = new ot::GraphicsHBoxLayoutItemCfg();

	myLayout->setMinimumSize(ot::Size2DD(150.0, 150.0));
	myStack->addItemTop(myLayout, true, false);

	// Now i want connections on the item for this i need rectangle items
	ot::GraphicsEllipseItemCfg* connection1 = new ot::GraphicsEllipseItemCfg();
	connection1->setName("Left4");
	ot::FillPainter2D* painter1 = new ot::FillPainter2D(ot::Color(ot::Blue));
	connection1->setOutline(ot::PenFCfg(1., ot::Color(ot::Black)));
	connection1->setBackgroundPainer(painter1);
	connection1->setAlignment(ot::Alignment::Center);
	connection1->setMaximumSize(ot::Size2DD(10.0, 10.0));

	ot::GraphicsEllipseItemCfg* connection2 = new ot::GraphicsEllipseItemCfg();
	connection2->setName("Right4");
	ot::FillPainter2D* painter2 = new ot::FillPainter2D(ot::Color(ot::Blue));
	connection2->setOutline(ot::PenFCfg(1., ot::Color(ot::Black)));
	connection2->setBackgroundPainer(painter2);
	connection2->setAlignment(ot::Alignment::Center);
	connection2->setMaximumSize(ot::Size2DD(10.0, 10.0));

	connection1->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsConnectable);
	connection2->setGraphicsItemFlags(ot::GraphicsItemCfg::ItemIsConnectable);


	//Here i add them to the Layout
	myLayout->addChildItem(connection1);
	myLayout->addStrech(1);
	myLayout->addChildItem(connection2);



	return myStack;
}

bool EntityBlockCircuitVoltageMeter::updateFromProperties(void)
{
	bool refresh = false;
	refresh = EntityBlockCircuitElement::updateFromProperties();

	if (refresh) {
		getProperties().forceResetUpdateForAllProperties();

	}
	
	return refresh;
}

void EntityBlockCircuitVoltageMeter::addStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBlock::addStorageData(storage);
}

void EntityBlockCircuitVoltageMeter::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBlock::readSpecificDataFromDataBase(doc_view, entityMap);
}