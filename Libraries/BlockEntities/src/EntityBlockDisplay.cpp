// @otlicense
// File: EntityBlockDisplay.cpp
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

#include "EntityBlockDisplay.h"
#include "SharedResources.h"

static EntityFactoryRegistrar<EntityBlockDisplay> registrar(EntityBlockDisplay::className());

EntityBlockDisplay::EntityBlockDisplay(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner)
	:EntityBlock(ID, parent, obs, ms, owner)
{
	OldTreeIcon icon;
	icon.visibleIcon = BlockEntities::SharedResources::getCornerImagePath() + getIconName();
	icon.hiddenIcon = BlockEntities::SharedResources::getCornerImagePath() + getIconName();
	setNavigationTreeIcon(icon);

	setBlockTitle("Display");

	_inputConnector = { ot::ConnectorType::In, "Input", "Input"};
	addConnector(_inputConnector);

	resetModified();
}

void EntityBlockDisplay::createProperties()
{
	EntityPropertiesString::createProperty("Display", "Description", "", "default", getProperties());
}

const std::string& EntityBlockDisplay::getDescription()
{
	EntityPropertiesBase* baseProperty = getProperties().getProperty("Description");
	EntityPropertiesString* stringProperty = dynamic_cast<EntityPropertiesString*>(baseProperty);
	return stringProperty->getValue();
}

ot::GraphicsItemCfg* EntityBlockDisplay::createBlockCfg()
{
	ot::GraphicsFlowItemBuilder block;
	block.setName(this->getClassName());
	block.setTitle(this->createBlockHeadline());
	
	const ot::Color colourTitle(ot::Yellow);
	block.setTitleBackgroundGradientColor(colourTitle);
	block.setLeftTitleCornerImagePath(BlockEntities::SharedResources::getCornerImagePath() + BlockEntities::SharedResources::getCornerImageNameVis());
	block.setBackgroundImagePath(BlockEntities::SharedResources::getCornerImagePath() + getIconName());
	addConnectors(block);

	ot::GraphicsItemCfg* graphicsItemConfig = block.createGraphicsItem();
	return graphicsItemConfig;
}
