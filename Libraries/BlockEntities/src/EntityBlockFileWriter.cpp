// @otlicense
// File: EntityBlockFileWriter.cpp
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

#include "EntityBlockFileWriter.h"
#include "SharedResources.h"
#include "PropertyHelper.h"

static EntityFactoryRegistrar<EntityBlockFileWriter> registrar(EntityBlockFileWriter::className());

EntityBlockFileWriter::EntityBlockFileWriter(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms)
	:EntityBlock(ID, parent, obs, ms)
{
	OldTreeIcon icon;
	icon.visibleIcon = BlockEntities::SharedResources::getCornerImagePath() + getIconName();
	icon.hiddenIcon = BlockEntities::SharedResources::getCornerImagePath() + getIconName();
	setNavigationTreeIcon(icon);

	setBlockTitle("File Writer");

	m_inputConnector = { ot::ConnectorType::In, "Input", "Input" };
	addConnector(m_inputConnector);

	resetModified();
}

void EntityBlockFileWriter::createProperties()
{
	EntityPropertiesString::createProperty("Headline", "Description", "", "default", getProperties());
	//sEntityPropertiesString::createProperty("File Properties", "Path", "", "default", getProperties());
	EntityPropertiesString::createProperty("File Properties", "File Name", "", "default", getProperties());
	
	EntityPropertiesSelection::createProperty("File Properties", "File type", { "txt", "json", "csv"}, "txt", "default", getProperties());
	//EntityPropertiesSelection::createProperty("File Properties", "Mode", { m_fileModeAppend, m_fileModeOverride }, m_fileModeAppend,"default",getProperties());
}

const std::string& EntityBlockFileWriter::getHeadline()
{
	auto descriptionPropertyBase = getProperties().getProperty("Description");
	assert(descriptionPropertyBase != nullptr);
	auto descriptionProperty = dynamic_cast<EntityPropertiesString*>(descriptionPropertyBase);
	assert(descriptionProperty != nullptr);
	return descriptionProperty->getValue();
}


const std::string& EntityBlockFileWriter::getFileName()
{
	auto propertyBase = getProperties().getProperty("File Name");
	assert(propertyBase != nullptr);
	auto propertyString = dynamic_cast<EntityPropertiesString*>(propertyBase);
	assert(propertyString != nullptr);
	return propertyString->getValue();
}


ot::GraphicsItemCfg* EntityBlockFileWriter::createBlockCfg()
{
	ot::GraphicsFlowItemBuilder block;
	block.setName(this->getClassName());
	block.setTitle(this->createBlockHeadline());

	const ot::Color colourTitle(ot::Yellow);
	block.setTitleBackgroundGradientColor(colourTitle);
	block.setLeftTitleCornerImagePath(BlockEntities::SharedResources::getCornerImagePath() + BlockEntities::SharedResources::getCornerImageNameDB());
	block.setBackgroundImagePath(BlockEntities::SharedResources::getCornerImagePath() + getIconName());
	
	addConnectors(block);

	ot::GraphicsItemCfg* graphicsItemConfig = block.createGraphicsItem();
	return graphicsItemConfig;
}

const std::string EntityBlockFileWriter::getFileType()
{
	return PropertyHelper::getSelectionPropertyValue(this, "File type");
}

