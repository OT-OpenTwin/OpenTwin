#include "EntityBlockDatabaseAccess.h"
#include "OTCommunication/ActionTypes.h"
#include "EntityBlockDatabaseAccess.h"
#include "EntityBlockConnection.h"	
#include "SharedResources.h"

EntityBlockDatabaseAccess::EntityBlockDatabaseAccess(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	:EntityBlock(ID, parent, obs, ms, factory, owner)
{
	m_navigationOldTreeIconName = BlockEntities::SharedResources::getCornerImagePath() + getIconName();
	m_navigationOldTreeIconNameHidden = BlockEntities::SharedResources::getCornerImagePath() + getIconName();
	m_blockTitle = "Database Access";


	const std::string connectorNameQuantity = "Quantity";
	m_connectorQuantity = { ot::ConnectorType::Out, connectorNameQuantity, connectorNameQuantity };
	m_connectorsByName[connectorNameQuantity] = m_connectorQuantity;

	const std::string connectorNameParameter1 = "Parameter1";
	const std::string connectorTitleParameter1 = "Parameter 1";
	m_connectorParameter1 = { ot::ConnectorType::Out,connectorNameParameter1, connectorTitleParameter1 };

	const std::string connectorNameParameter2 = "Parameter2";
	const std::string connectorTitleParameter2 = "Parameter 2";
	m_connectorParameter2 = { ot::ConnectorType::Out,connectorNameParameter2, connectorTitleParameter2 };

	const std::string connectorNameParameter3 = "Parameter3";
	const std::string connectorTitleParameter3 = "Parameter 3";
	m_connectorParameter3 = { ot::ConnectorType::Out,connectorNameParameter3, connectorTitleParameter3 };

	m_connectorsByName[connectorNameParameter1] = m_connectorParameter1;
}

void EntityBlockDatabaseAccess::createProperties(std::list<std::string>& comparators)
{
	EntityPropertiesProjectList* projectList = new EntityPropertiesProjectList("Projectname");
	getProperties().createProperty(projectList, "Database");

	//Basic properties:
	EntityPropertiesSelection::createProperty(m_groupQuerySettings, m_propertyNameDimension, { m_propertyValueDimension1,m_propertyValueDimension2, m_propertyValueDimension3, m_propertyValueDimensionCustom }, m_propertyValueDimension1, "default", getProperties());
	EntityPropertiesSelection::createProperty(m_groupQuerySettings, m_propertyNameSeriesMetadata, { ""}, "", "default", getProperties());
	
	//Quantity Settings
	std::list<std::string> groupNames{ m_groupQuantitySetttings,m_groupParamSettings1,m_groupParamSettings2,m_groupParamSettings3 };

	std::list<std::string>::iterator groupName = groupNames.begin();
	for (; groupName != groupNames.end(); groupName++)
	{
		EntityPropertiesSelection::createProperty(*groupName, m_propertyName, {""}, "", "default", getProperties());

		if (*groupName == m_groupQuantitySetttings)
		{
			EntityPropertiesSelection::createProperty(m_groupQuantitySetttings, m_propertyValueDescription, { "" }, "", "default", getProperties());
		}

		EntityPropertiesString* typeLabelProperty = new EntityPropertiesString();
		typeLabelProperty->setReadOnly(true);
		typeLabelProperty->setName(m_propertyDataType);
		typeLabelProperty->setGroup(*groupName);
		typeLabelProperty->setValue("");
		getProperties().createProperty(typeLabelProperty, *groupName);

		EntityPropertiesString* unitLabelProperty = new EntityPropertiesString();
		unitLabelProperty->setReadOnly(true);
		unitLabelProperty->setName(m_propertyUnit);
		unitLabelProperty->setGroup(*groupName);
		unitLabelProperty->setValue("");
		getProperties().createProperty(unitLabelProperty, *groupName);


		EntityPropertiesString::createProperty(*groupName, m_propertyValue, "", "default", getProperties());
		EntityPropertiesSelection::createProperty(*groupName, m_propertyComparator, comparators, comparators.back(), "default", getProperties());
	}
	setVisibleParameter2(false);
	setVisibleParameter3(false);
}

void EntityBlockDatabaseAccess::setSelectionSeries(std::list<std::string>& _options, const std::string& _selectedValue)
{
	EntityPropertiesBase* baseEntity =	getProperties().getProperty(m_propertyNameSeriesMetadata, m_groupQuerySettings);
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

std::string EntityBlockDatabaseAccess::getQueryDimension()
{
	auto propertyBase = getProperties().getProperty("Outcome dimension");
	auto outcome = dynamic_cast<EntityPropertiesSelection*>(propertyBase);
	assert(outcome != nullptr);

	return outcome->getValue();
}

const bool EntityBlockDatabaseAccess::isQueryDimension1D()
{
	std::string selectedValue = getQueryDimension();
	return selectedValue == m_propertyValueDimension1;
}

const bool EntityBlockDatabaseAccess::isQueryDimension2D()
{
	std::string selectedValue = getQueryDimension();
	return selectedValue == m_propertyValueDimension2;
}

const bool EntityBlockDatabaseAccess::isQueryDimension3D()
{
	std::string selectedValue = getQueryDimension();
	return selectedValue == m_propertyValueDimension3;
}

bool EntityBlockDatabaseAccess::setVisibleParameter2(bool _visible)
{
	return setVisibleParameter(m_groupParamSettings2,_visible);
}

bool EntityBlockDatabaseAccess::setVisibleParameter3(bool _visible)
{
	return setVisibleParameter(m_groupParamSettings3, _visible);
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
	bool updateOfDimensionality = getProperties().getProperty(m_propertyNameDimension, m_groupQuerySettings)->needsUpdate();
	if (updateOfDimensionality)
	{
		const std::string queryDimension = getQueryDimension();
		std::list<std::string> connectorsForRemoval;
		if (m_propertyValueDimension2 == queryDimension)
		{
			if (m_connectorsByName.find(m_connectorParameter3.getConnectorName()) != m_connectorsByName.end())
			{
				m_connectorsByName.erase(m_connectorParameter3.getConnectorName());
				connectorsForRemoval.push_back(m_connectorParameter3.getConnectorName());
			}
			if (m_connectorsByName.find(m_connectorParameter2.getConnectorName()) == m_connectorsByName.end())
			{
				m_connectorsByName[m_connectorParameter2.getConnectorName()] = m_connectorParameter2;
			}
		}
		else if (m_propertyValueDimension3 == queryDimension)
		{
			if (m_connectorsByName.find(m_connectorParameter2.getConnectorName()) == m_connectorsByName.end())
			{
				m_connectorsByName[m_connectorParameter2.getConnectorName()] = m_connectorParameter2;
			}
			if (m_connectorsByName.find(m_connectorParameter3.getConnectorName()) == m_connectorsByName.end())
			{
				m_connectorsByName[m_connectorParameter3.getConnectorName()] = m_connectorParameter3;
			}
		}
		else
		{
			assert(m_propertyValueDimension1 == queryDimension);
			if (m_connectorsByName.find(m_connectorParameter3.getConnectorName()) != m_connectorsByName.end())
			{
				m_connectorsByName.erase(m_connectorParameter3.getConnectorName());
				connectorsForRemoval.push_back(m_connectorParameter3.getConnectorName());
			}
			if (m_connectorsByName.find(m_connectorParameter2.getConnectorName()) != m_connectorsByName.end())
			{
				m_connectorsByName.erase(m_connectorParameter2.getConnectorName());
				connectorsForRemoval.push_back(m_connectorParameter2.getConnectorName());
			}
		}

		CreateBlockItem();
		updateConnections(connectorsForRemoval);
	}
}


void EntityBlockDatabaseAccess::updateConnections(std::list<std::string>& connectorsForRemoval)
{
	std::list<ot::UID> connectionsForRemoval;
	std::list<ot::UID> remainingConnections;
	std::map<ot::UID, EntityBase*> entityMap;
	for (auto& connectionID : m_connectionIDs)
	{
		EntityBase* entityBase = readEntityFromEntityID(this, connectionID, entityMap);
		EntityBlockConnection* connectionEntity = dynamic_cast<EntityBlockConnection*>(entityBase);
		ot::GraphicsConnectionCfg connection = connectionEntity->getConnectionCfg();
		std::string connectorOfThisBlock;

		if (connection.getDestinationUid() == getEntityID())
		{
			connectorOfThisBlock = connection.getDestConnectable();
		}
		else
		{
			connectorOfThisBlock = connection.getOriginConnectable();
		}
		bool removeConnection = false;

		for (std::string& connectorName : connectorsForRemoval)
		{
			if (connectorName == connectorOfThisBlock)
			{
				removeConnection = true;
				break;
			}
		}
		if (removeConnection)
		{
			connectionsForRemoval.push_back(connectionEntity->getEntityID());
		}
		else
		{
			remainingConnections.push_back(connectionEntity->getEntityID());
		}

	}
	m_connectionIDs = std::move(remainingConnections);
	removeConnectionsAtConnectedEntities(connectionsForRemoval);
}

void EntityBlockDatabaseAccess::removeConnectionsAtConnectedEntities(std::list<ot::UID>& connectionsForRemoval)
{
	if (connectionsForRemoval.size() != 0)
	{
		std::map<ot::UID, EntityBase*> entityMapFirst;
		ot::UIDList entitiesForUpdate;
		std::map<ot::UID, std::list<ot::GraphicsConnectionCfg*>> connectionsForRemovalByEntityID;

		for (auto& connectionID : connectionsForRemoval)
		{
			EntityBase* entityBase = readEntityFromEntityID(this, connectionID, entityMapFirst);
			EntityBlockConnection* connectionEntity = dynamic_cast<EntityBlockConnection*>(entityBase);
			ot::GraphicsConnectionCfg connection = connectionEntity->getConnectionCfg();

			if (connection.getDestinationUid() == getEntityID())
			{
				entitiesForUpdate.push_back(connection.getOriginUid());
			}
			else
			{
				entitiesForUpdate.push_back(connection.getDestinationUid());
			}
			connectionsForRemovalByEntityID[entitiesForUpdate.back()].push_back(&connection);
		}
		std::map<ot::UID, EntityBase*> entityMapSecond;
		for (const ot::UID& entityID : entitiesForUpdate)
		{
			EntityBase* entityBase = readEntityFromEntityID(nullptr, entityID, entityMapSecond);
			assert(entityBase != nullptr);
			std::unique_ptr<EntityBlock> blockEntity(dynamic_cast<EntityBlock*>(entityBase));
			assert(blockEntity != nullptr);

			for (auto connection : connectionsForRemovalByEntityID[entityID])
			{
				blockEntity->RemoveConnection(entityID);
			}
			blockEntity->StoreToDataBase();
			getModelState()->modifyEntityVersion(blockEntity->getEntityID(), blockEntity->getEntityStorageVersion());
		}
	}
}

ot::GraphicsItemCfg* EntityBlockDatabaseAccess::CreateBlockCfg()
{
	ot::GraphicsFlowItemBuilder block;
	block.setName(this->getClassName());
	block.setTitle(this->CreateBlockHeadline());

	const ot::Color colourTitle(ot::Lime);
	const ot::Color colourBackground(ot::White);
	block.setTitleBackgroundGradientColor(colourTitle);
	block.setLeftTitleCornerImagePath(BlockEntities::SharedResources::getCornerImagePath() + BlockEntities::SharedResources::getCornerImageNameDB());
	block.setBackgroundImagePath(BlockEntities::SharedResources::getCornerImagePath() + getIconName());

	AddConnectors(block);

	auto graphicsItemConfig = block.createGraphicsItem();
	return graphicsItemConfig;
}

bool EntityBlockDatabaseAccess::updateFromProperties()
{
	auto baseProperty = getProperties().getProperty(m_propertyNameDimension);
	auto selectionProperty = dynamic_cast<EntityPropertiesSelection*>(baseProperty);
	bool refresh = false;
	if (selectionProperty->getValue() == m_propertyValueDimension1)
	{
		refresh = setVisibleParameter2(false);
		refresh |= setVisibleParameter3(false);
	}
	else if (selectionProperty->getValue() == m_propertyValueDimension2)
	{
		refresh = setVisibleParameter2(true);
		refresh |= setVisibleParameter3(false);
	}
	else if (selectionProperty->getValue() == m_propertyValueDimension3)
	{
		refresh = setVisibleParameter2(true);
		refresh |= setVisibleParameter3(true);
	}
	
	updateBlockConfig();

	if (refresh)
	{
		getProperties().forceResetUpdateForAllProperties();
	}
	return refresh;
}

void EntityBlockDatabaseAccess::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBlock::AddStorageData(storage);
}

void EntityBlockDatabaseAccess::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBlock::readSpecificDataFromDataBase(doc_view, entityMap);
}

EntityPropertiesSelection* EntityBlockDatabaseAccess::getSeriesSelection()
{
	EntityPropertiesBase* base = getProperties().getProperty(m_propertyNameSeriesMetadata, m_groupQuerySettings);
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

ValueCharacteristicProperties EntityBlockDatabaseAccess::getParameterValueCharacteristic1()
{
	return getValueCharacteristics(m_groupParamSettings1);
}

ValueCharacteristicProperties EntityBlockDatabaseAccess::getParameterValueCharacteristic2()
{
	return getValueCharacteristics(m_groupParamSettings2);
}

ValueCharacteristicProperties EntityBlockDatabaseAccess::getParameterValueCharacteristic3()
{
	return getValueCharacteristics(m_groupParamSettings3);
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

const ValueComparisionDefinition EntityBlockDatabaseAccess::getSelectedParameter1Definition()
{
	return getSelectedValueComparisionDefinition(m_groupParamSettings1);
}

const ValueComparisionDefinition EntityBlockDatabaseAccess::getSelectedParameter2Definition()
{
	return getSelectedValueComparisionDefinition(m_groupParamSettings2);
}

const ValueComparisionDefinition EntityBlockDatabaseAccess::getSelectedParameter3Definition()
{
	return getSelectedValueComparisionDefinition(m_groupParamSettings3);
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
