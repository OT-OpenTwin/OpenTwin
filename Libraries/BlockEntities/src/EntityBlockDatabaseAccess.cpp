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

#include "EntityBlockDatabaseAccess.h"
#include "OTCommunication/ActionTypes.h"
#include "EntityBlockConnection.h"	
#include "SharedResources.h"
#include "PropertyHelper.h"
#include "OTCore/ComparisionSymbols.h"

static EntityFactoryRegistrar<EntityBlockDatabaseAccess> registrar(EntityBlockDatabaseAccess::className());

EntityBlockDatabaseAccess::EntityBlockDatabaseAccess(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner)
	:EntityBlock(ID, parent, obs, ms, owner)
{
	OldTreeIcon icon;
	icon.visibleIcon = BlockEntities::SharedResources::getCornerImagePath() + getIconName();
	icon.hiddenIcon = BlockEntities::SharedResources::getCornerImagePath() + getIconName();
	setNavigationTreeIcon(icon);

	setBlockTitle("Database Access");

	const std::string connectorNameOutput= "Output";
	m_connectorOutput = { ot::ConnectorType::Out, connectorNameOutput, connectorNameOutput };
	addConnector(m_connectorOutput);

	resetModified();
}

void EntityBlockDatabaseAccess::createProperties()
{
	std::list<std::string> comparators = ot::ComparisionSymbols::g_comparators;
	comparators.push_front("");

	EntityPropertiesProjectList* projectList = new EntityPropertiesProjectList("Projectname");
	getProperties().createProperty(projectList, m_groupMetadataFilter);

	//Basic properties:
	EntityPropertiesSelection::createProperty(m_groupMetadataFilter, m_propertyNameSeriesMetadata, { ""}, "", "default", getProperties());
	EntityPropertiesInteger::createProperty(m_groupMetadataFilter, m_propertyNumberOfQueries, 0,0, m_maxNbOfQueries, "default", getProperties());
	
	//Quantity Settings
	EntityPropertiesSelection::createProperty(m_groupQuantitySetttings, m_propertyName, {""}, "", "default", getProperties());
	EntityPropertiesSelection::createProperty(m_groupQuantitySetttings, m_propertyValueDescription, { "" }, "", "default", getProperties());
	EntityPropertiesString* typeLabelProperty = new EntityPropertiesString();
	typeLabelProperty->setReadOnly(true);
	typeLabelProperty->setName(m_propertyDataType);
	typeLabelProperty->setGroup(m_groupQuantitySetttings);
	typeLabelProperty->setValue("");
	getProperties().createProperty(typeLabelProperty, m_groupQuantitySetttings);

	EntityPropertiesString* unitLabelProperty = new EntityPropertiesString();
	unitLabelProperty->setReadOnly(true);
	unitLabelProperty->setName(m_propertyUnit);
	unitLabelProperty->setGroup(m_groupQuantitySetttings);
	unitLabelProperty->setValue("");
	getProperties().createProperty(unitLabelProperty, m_groupQuantitySetttings);

	EntityPropertiesString::createProperty(m_groupQuantitySetttings, m_propertyValue, "", "default", getProperties());
	EntityPropertiesSelection::createProperty(m_groupQuantitySetttings, m_propertyComparator, comparators, comparators.front(), "default", getProperties());

	EntityPropertiesBoolean::createProperty(m_groupQuerySetttings, "Order reproducable", true,"default", getProperties());

	//Add all query fields
	for (uint32_t i = 1; i <= m_maxNbOfQueries; i++)
	{
		const std::string groupName = m_groupQuerySetttings + "_" + std::to_string(i);

		EntityPropertiesSelection::createProperty(groupName, m_propertyName, { "" }, "", "default", getProperties());
		PropertyHelper::getSelectionProperty(this, m_propertyName, groupName)->setVisible(false);

		EntityPropertiesString* typeLabelProperty = new EntityPropertiesString();
		typeLabelProperty->setReadOnly(true);
		typeLabelProperty->setName(m_propertyDataType);
		typeLabelProperty->setGroup(groupName);
		typeLabelProperty->setValue("");
		getProperties().createProperty(typeLabelProperty, groupName);
		PropertyHelper::getStringProperty(this, m_propertyDataType, groupName)->setVisible(false);

		EntityPropertiesString* unitLabelProperty = new EntityPropertiesString();
		unitLabelProperty->setReadOnly(true);
		unitLabelProperty->setName(m_propertyUnit);
		unitLabelProperty->setGroup(groupName);
		unitLabelProperty->setValue("");
		getProperties().createProperty(unitLabelProperty, groupName);
		PropertyHelper::getStringProperty(this, m_propertyUnit, groupName)->setVisible(false);

		EntityPropertiesString::createProperty(groupName, m_propertyValue, "", "default", getProperties());
		PropertyHelper::getStringProperty(this, m_propertyValue, groupName)->setVisible(false);

		EntityPropertiesSelection::createProperty(groupName, m_propertyComparator, comparators, comparators.front(), "default", getProperties());
		PropertyHelper::getSelectionProperty(this, m_propertyComparator, groupName)->setVisible(false);

	}	
}

void EntityBlockDatabaseAccess::setSelectionSeries(std::list<std::string>& _options, const std::string& _selectedValue)
{
	EntityPropertiesBase* baseEntity =	getProperties().getProperty(m_propertyNameSeriesMetadata, m_groupMetadataFilter);
	assert(baseEntity != nullptr);
	EntityPropertiesSelection* selectionEntity = dynamic_cast<EntityPropertiesSelection*>(baseEntity);
	assert(selectionEntity != nullptr);

	selectionEntity->resetOptions(_options);
	selectionEntity->setValue(_selectedValue);
}

std::string EntityBlockDatabaseAccess::getSelectedProjectName()
{
	auto propertyBase = getProperties().getProperty(m_propertyNameProjectName);
	auto selectedProjectName = dynamic_cast<EntityPropertiesProjectList*>(propertyBase);
	assert(selectedProjectName != nullptr);

	return  selectedProjectName->getValue();
}

bool EntityBlockDatabaseAccess::setVisibleParameter(const std::string& _groupName, bool _visible)
{
	bool isVisible = getProperties().getProperty(m_propertyName, _groupName)->getVisible();
	const bool refresh = isVisible != _visible;
	if (refresh)
	{
		getProperties().getProperty(m_propertyName, _groupName)->setVisible(_visible);
		getProperties().getProperty(m_propertyDataType, _groupName)->setVisible(_visible);
		getProperties().getProperty(m_propertyComparator, _groupName)->setVisible(_visible);
		getProperties().getProperty(m_propertyValue, _groupName)->setVisible(_visible);
		getProperties().getProperty(m_propertyUnit, _groupName)->setVisible(_visible);
		this->setModified();
	}
	return refresh;
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
	block.setLeftTitleCornerImagePath(BlockEntities::SharedResources::getCornerImagePath() + BlockEntities::SharedResources::getCornerImageNameDB());
	block.setBackgroundImagePath(BlockEntities::SharedResources::getCornerImagePath() + getIconName());

	addConnectors(block);

	auto graphicsItemConfig = block.createGraphicsItem();
	return graphicsItemConfig;
}

bool EntityBlockDatabaseAccess::getReproducableOrder()
{
	const bool orderReproducable = PropertyHelper::getBoolPropertyValue(this, "Order reproducable", m_groupQuerySetttings);
	return orderReproducable;
}

bool EntityBlockDatabaseAccess::updateFromProperties()
{

	EntityPropertiesInteger* numberOfQueriesProp =PropertyHelper::getIntegerProperty(this, m_propertyNumberOfQueries);
	bool requiresUpdate = numberOfQueriesProp->needsUpdate();
	if (requiresUpdate)
	{
		uint32_t numberVisible = static_cast<uint32_t>(numberOfQueriesProp->getValue());
		for (uint32_t i = 1; i <= m_maxNbOfQueries; i++)
		{
			const std::string groupName = m_groupQuerySetttings + "_" + std::to_string(i);
			PropertyHelper::getStringProperty(this, m_propertyDataType, groupName)->setVisible(i <= numberVisible);
			PropertyHelper::getStringProperty(this, m_propertyUnit, groupName)->setVisible(i <= numberVisible);
			PropertyHelper::getStringProperty(this, m_propertyValue, groupName)->setVisible(i <= numberVisible);
			PropertyHelper::getSelectionProperty(this, m_propertyName, groupName)->setVisible(i <= numberVisible);
			PropertyHelper::getSelectionProperty(this, m_propertyComparator, groupName)->setVisible(i <= numberVisible);
		}
	}


	//updateBlockConfig();

	return requiresUpdate;
}

void EntityBlockDatabaseAccess::addStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBlock::addStorageData(storage);
}

void EntityBlockDatabaseAccess::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBlock::readSpecificDataFromDataBase(doc_view, entityMap);
}

EntityPropertiesSelection* EntityBlockDatabaseAccess::getSeriesSelection()
{
	EntityPropertiesBase* base = getProperties().getProperty(m_propertyNameSeriesMetadata, m_groupMetadataFilter);
	assert(base != nullptr);
	EntityPropertiesSelection* selection = dynamic_cast<EntityPropertiesSelection*>(base);
	assert(selection != nullptr);
	return selection;
}

EntityPropertiesSelection* EntityBlockDatabaseAccess::getQuantityValueDescriptionSelection()
{
	EntityPropertiesBase* base = getProperties().getProperty(m_propertyValueDescription, m_groupQuantitySetttings);
	assert(base != nullptr);
	EntityPropertiesSelection* selection = dynamic_cast<EntityPropertiesSelection*>(base);
	assert(selection != nullptr);
	return selection;
}

ValueCharacteristicProperties EntityBlockDatabaseAccess::getQuantityValueCharacteristic()
{
	return getValueCharacteristics(m_groupQuantitySetttings);
}

ValueCharacteristicProperties EntityBlockDatabaseAccess::getValueCharacteristics(const std::string& _groupName)
{
	ValueCharacteristicProperties valueCharacteristicProperties;
	EntityPropertiesBase* base = getProperties().getProperty(m_propertyName, _groupName);
	assert(base != nullptr);
	EntityPropertiesSelection* selection = dynamic_cast<EntityPropertiesSelection*>(base);
	assert(selection != nullptr);
	valueCharacteristicProperties.m_label = selection;

	base = getProperties().getProperty(m_propertyUnit, _groupName);
	assert(base != nullptr);
	EntityPropertiesString* unitProperty = dynamic_cast<EntityPropertiesString*>(base);
	valueCharacteristicProperties.m_unit = unitProperty;

	base = getProperties().getProperty(m_propertyDataType, _groupName);
	assert(base != nullptr);
	EntityPropertiesString* dataTypeProperty = dynamic_cast<EntityPropertiesString*>(base);
	valueCharacteristicProperties.m_dataType = dataTypeProperty;

	return valueCharacteristicProperties;
}

ValueCharacteristicProperties EntityBlockDatabaseAccess::getQueryValueCharacteristics(int32_t _queryIndex)
{
	const std::string groupName = m_groupQuerySetttings + "_" + std::to_string(_queryIndex);
	return getValueCharacteristics(groupName);
}

int32_t EntityBlockDatabaseAccess::getSelectedNumberOfQueries()
{
	int32_t numberOfQueries = PropertyHelper::getIntegerPropertyValue(this, m_propertyNumberOfQueries);
	return numberOfQueries;
}


void EntityBlockDatabaseAccess::createUpdatedProperty(const std::string& _propName, const std::string& _propGroup, const std::string& _labelValue, EntityProperties& _properties)
{
	EntityPropertiesBase* base = getProperties().getProperty(_propName, _propGroup);
	EntityPropertiesString* label = dynamic_cast<EntityPropertiesString*>(base);
	EntityPropertiesString* newLabel = new EntityPropertiesString();
	*newLabel = *label;
	newLabel->setValue(_labelValue);
	_properties.createProperty(newLabel, newLabel->getGroup());
}

const ValueComparisionDefinition EntityBlockDatabaseAccess::getSelectedQuantityDefinition()
{
	return getSelectedValueComparisionDefinition(m_groupQuantitySetttings);
}

const std::list<ValueComparisionDefinition> EntityBlockDatabaseAccess::getAdditionalQueries()
{
	
	std::list<ValueComparisionDefinition> valueComparisionDefinitions;
	const int32_t numberOfQueries = getSelectedNumberOfQueries();
	for (int i = 1; i <= numberOfQueries; i++)
	{
		const std::string groupName = m_groupQuerySetttings + "_" + std::to_string(i);
		const std::string dataType = PropertyHelper::getStringPropertyValue(this, m_propertyDataType, groupName);
		const std::string unit = PropertyHelper::getStringPropertyValue(this, m_propertyUnit, groupName);
		const std::string value = PropertyHelper::getStringPropertyValue(this, m_propertyValue, groupName);
		const std::string name = PropertyHelper::getSelectionPropertyValue(this, m_propertyName, groupName);
		const std::string comparator = PropertyHelper::getSelectionPropertyValue(this, m_propertyComparator, groupName);
		const ValueComparisionDefinition valueComparisionDefinition(name, comparator, value, dataType, unit);
		valueComparisionDefinitions.push_back(valueComparisionDefinition);
	}
	return valueComparisionDefinitions;
}

const ValueComparisionDefinition EntityBlockDatabaseAccess::getSelectedValueComparisionDefinition(const std::string& _groupName)
{
	auto baseProp = getProperties().getProperty(m_propertyName,_groupName);
	auto nameProp = dynamic_cast<EntityPropertiesSelection*>(baseProp);
	const std::string name = nameProp->getValue();

	baseProp = getProperties().getProperty(m_propertyValue,_groupName);
	auto valueProp = dynamic_cast<EntityPropertiesString*>(baseProp);
	const std::string value = valueProp->getValue();

	baseProp = getProperties().getProperty(m_propertyComparator, _groupName);
	auto comparatorProp = dynamic_cast<EntityPropertiesSelection*>(baseProp);
	const std::string comparator = comparatorProp->getValue();

	baseProp = getProperties().getProperty(m_propertyDataType,_groupName);
	auto typeProp = dynamic_cast<EntityPropertiesString*>(baseProp);
	const std::string type = typeProp->getValue();

	baseProp = getProperties().getProperty(m_propertyUnit, _groupName);
	auto unitProp = dynamic_cast<EntityPropertiesString*>(baseProp);
	const std::string unit = typeProp->getValue();

	return ValueComparisionDefinition(name, comparator, value, type,unit);
}
