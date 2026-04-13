// @otlicense
// File: EntityBlockDatabaseAccess.cpp
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
#include "OTCore/ComparisonSymbols.h"
#include "OTCommunication/ActionTypes.h"
#include "OTModelEntities/PropertyHelper.h"
#include "OTBlockEntities/BlockImageNames.h"
#include "OTBlockEntities/EntityBlockConnection.h"	
#include "OTBlockEntities/Pipeline/EntityBlockDatabaseAccess.h"

static EntityFactoryRegistrar<EntityBlockDatabaseAccess> registrar(EntityBlockDatabaseAccess::className());

EntityBlockDatabaseAccess::EntityBlockDatabaseAccess(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms)
	:EntityBlock(ID, parent, obs, ms)
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon(ot::BlockImageNames::getCornerImagePath() + getIconName());
	treeItem.setHiddenIcon(ot::BlockImageNames::getCornerImagePath() + getIconName());
	this->setDefaultTreeItem(treeItem);
	
	setBlockTitle("Database Access");

	const std::string connectorNameOutput= "Output";
	m_connectorOutput = { ot::ConnectorType::Out, connectorNameOutput, connectorNameOutput };
	addConnector(m_connectorOutput);

	resetModified();
}

void EntityBlockDatabaseAccess::createProperties()
{
	m_propertyBundleDataLakeQuery.setProperties(this);
}

std::string EntityBlockDatabaseAccess::getSelectedProjectName()
{
	return  m_propertyBundleDataLakeQuery.getSelectedProject(this);
}

void EntityBlockDatabaseAccess::updateBlockConfig()
{

}

ot::GraphicsItemCfg* EntityBlockDatabaseAccess::createBlockCfg()
{
	ot::GraphicsFlowItemBuilder block;
	block.setName(this->getClassName());
	block.setTitle(this->createBlockHeadline());

	const ot::Color colourTitle(ot::Lime);
	const ot::Color colourBackground(ot::White);
	block.setTitleBackgroundGradientColor(colourTitle);
	block.setLeftTitleCornerImagePath(ot::BlockImageNames::getCornerImagePath() + ot::BlockImageNames::getCornerImageNameDB());
	block.setBackgroundImagePath(ot::BlockImageNames::getCornerImagePath() + getIconName());

	addConnectors(block);

	auto graphicsItemConfig = block.createGraphicsItem();
	return graphicsItemConfig;
}

bool EntityBlockDatabaseAccess::getReproducibleOrder()
{
	//const bool orderReproducible = PropertyHelper::getBoolPropertyValue(this, m_propertyOrder, m_groupQuerySetttings);
	//return orderReproducible;
	return true;
}

bool EntityBlockDatabaseAccess::updateFromProperties()
{
	bool requiresUpdate = m_propertyBundleDataLakeQuery.updatePropertyVisibility(this);
	if (getObserver() != nullptr)
	{

		const std::string projectName = m_propertyBundleDataLakeQuery.getSelectedProject(this);
		std::string collectionName;
		auto associatedCampaign = getObserver()->getMetadataCampaign(projectName, collectionName);
		assert(associatedCampaign.has_value()); //Only not the case, if the observer has no implementation of the getter.
		requiresUpdate |= m_propertyBundleDataLakeQuery.updateOptions(this, associatedCampaign.value());

		DataLakeQueryCfg cfg;
		cfg.setCollectionName(collectionName);
		cfg.setSeriesLabel(m_propertyBundleDataLakeQuery.getSelectedSeries(this));
		cfg.setValueDescriptionParameters(m_propertyBundleDataLakeQuery.getParameterQueries(this));
		cfg.setValueDescriptionSeriesMD(m_propertyBundleDataLakeQuery.getMetadataQueries(this));
		cfg.setValueDescriptionQuantities(m_propertyBundleDataLakeQuery.getQuantityQuery(this));

		getObserver()->requestDatapointVisualisation(cfg, getEntityID(), getEntityStorageVersion());
	}
		
	getProperties().forceResetUpdateForAllProperties();
	//updateBlockConfig();

	return requiresUpdate;
}

void EntityBlockDatabaseAccess::addStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBlock::addStorageData(storage);
}

void EntityBlockDatabaseAccess::readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBlock::readSpecificDataFromDataBase(doc_view, entityMap);
}