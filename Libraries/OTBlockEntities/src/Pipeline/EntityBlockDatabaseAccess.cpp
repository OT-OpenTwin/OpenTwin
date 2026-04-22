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
#include "OTBlockEntities/BlockImageNames.h"
#include "OTBlockEntities/EntityBlockConnection.h"	
#include "OTModelEntities/Properties/PropertyHelper.h"
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
	m_queryProperties.setProperties(this);
	const std::string group = m_queryProperties.getQuerySettingsGroupName();
	EntityPropertiesBoolean::createProperty(group, m_propertyNameLimit, false, "default", getProperties());
	auto base = EntityPropertiesInteger::createProperty(group, m_propertyNameLimitNb, 500, "default", getProperties());
	base->setVisible(false);
}

std::string EntityBlockDatabaseAccess::getSelectedProjectName()
{
	return  m_queryProperties.getSelectedProject(this);
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

bool EntityBlockDatabaseAccess::getReproducibleOrder()const
{
	return m_queryProperties.getOrderReproducible(this);
}

std::pair<bool, uint32_t>  EntityBlockDatabaseAccess::getResultLimit() const
{
	const std::string settingsGroup = m_queryProperties.getQuerySettingsGroupName();
	int32_t limit =	PropertyHelper::getIntegerPropertyValue(this, m_propertyNameLimitNb, settingsGroup);
	bool useLimit = PropertyHelper::getBoolPropertyValue(this, m_propertyNameLimit, settingsGroup);
	return {useLimit,static_cast<uint32_t>(limit)};
}

bool EntityBlockDatabaseAccess::updateFromProperties()
{
	const std::string settingsGroup = m_queryProperties.getQuerySettingsGroupName();
	auto propertyLimitResults =	PropertyHelper::getBoolProperty(this, m_propertyNameLimit, settingsGroup);
	bool refresh = 	propertyLimitResults->needsUpdate();
	if (refresh)
	{
		PropertyHelper::getIntegerProperty(this, m_propertyNameLimitNb, settingsGroup)->setVisible(propertyLimitResults->getValue());
	}
	if (getObserver() != nullptr)
	{
		try
		{
			const std::string projectName = m_queryProperties.getSelectedProject(this);
			std::string collectionName;
			auto associatedCampaign = getObserver()->getMetadataCampaign(projectName, collectionName);

			assert(associatedCampaign.has_value()); //Only not the case, if the observer has no implementation of the getter.
			bool dataRefreshNeeded = m_queryProperties.updateOptions(this, associatedCampaign.value());
			refresh |= dataRefreshNeeded;

			if (dataRefreshNeeded)
			{
				DataLakeQueryCfg queryCfg;
				queryCfg.setCollectionName(collectionName);
				queryCfg.setSeriesLabel(m_queryProperties.getSelectedSeries(this));
				queryCfg.setValueDescriptionParameters(m_queryProperties.getParameterQueries(this));
				queryCfg.setValueDescriptionSeriesMD(m_queryProperties.getMetadataQueries(this));
				queryCfg.setValueDescriptionQuantities(m_queryProperties.getQuantityQuery(this));

				m_dataLakeAccessCfg = getObserver()->createDataLakeAccessConfig(associatedCampaign.value(), collectionName, queryCfg);
			}
		}
		catch (std::exception& _e)
		{
			std::string message = "Failed to use user given comparisons for data lake access: " + std::string(_e.what());
			OT_LOG_E(message);
		}
	}

	getProperties().forceResetUpdateForAllProperties();
	return refresh;
}

void EntityBlockDatabaseAccess::addStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBlock::addStorageData(storage);
	const std::string serialisedDLA = m_dataLakeAccessCfg.toJson();
	storage.append
	(
		bsoncxx::builder::basic::kvp("DataLakeAccessCfg", serialisedDLA)
	);
}

void EntityBlockDatabaseAccess::readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBlock::readSpecificDataFromDataBase(doc_view, entityMap);
	if (doc_view.find("DataLakeAccessCfg") != doc_view.end())
	{
		const std::string serialisedDLA = doc_view["DataLakeAccessCfg"].get_string().value.data();
		if (!serialisedDLA.empty())
		{
			ot::JsonDocument doc;
			doc.fromJson(serialisedDLA);
			m_dataLakeAccessCfg.setFromJsonObject(doc.getConstObject());
		}
	}
}