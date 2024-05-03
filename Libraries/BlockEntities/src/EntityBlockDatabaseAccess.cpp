#include "EntityBlockDatabaseAccess.h"
#include "OTCommunication/ActionTypes.h"
#include "EntityBlockDatabaseAccess.h"
#include "EntityBlockConnection.h"	


EntityBlockDatabaseAccess::EntityBlockDatabaseAccess(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	:EntityBlock(ID, parent, obs, ms, factory, owner)
{
	_navigationTreeIconName = "BlockDataBaseAccess";
	_navigationTreeIconNameHidden = "BlockDataBaseAccess";
	_blockTitle = "Database Access";


	const std::string connectorNameQuantity = "Quantity";
	_connectorQuantity = { ot::ConnectorType::Out, connectorNameQuantity, connectorNameQuantity };
	_connectorsByName[connectorNameQuantity] = _connectorQuantity;

	const std::string connectorNameParameter1 = "Parameter1";
	const std::string connectorTitleParameter1 = "Parameter 1";
	_connectorParameter1 = { ot::ConnectorType::Out,connectorNameParameter1, connectorTitleParameter1 };

	const std::string connectorNameParameter2 = "Parameter2";
	const std::string connectorTitleParameter2 = "Parameter 2";
	_connectorParameter2 = { ot::ConnectorType::Out,connectorNameParameter2, connectorTitleParameter2 };

	const std::string connectorNameParameter3 = "Parameter3";
	const std::string connectorTitleParameter3 = "Parameter 3";
	_connectorParameter3 = { ot::ConnectorType::Out,connectorNameParameter3, connectorTitleParameter3 };

	_connectorsByName[connectorNameParameter1] = _connectorParameter1;
}

void EntityBlockDatabaseAccess::createProperties(std::list<std::string>& comparators)
{
	EntityPropertiesProjectList* projectList = new EntityPropertiesProjectList("Projectname");
	getProperties().createProperty(projectList, "Database");

	//Basic properties:
	EntityPropertiesSelection::createProperty(_groupQuerySettings, _propertyNameDimension, { _propertyValueDimension1,_propertyValueDimension2, _propertyValueDimension3, _propertyValueDimensionCustom }, _propertyValueDimension1, "default", getProperties());

	//Quantity Settings
	EntityPropertiesSelection::createProperty(_groupQuantitySetttings, _propertyNameQuantity, {}, "", "default", getProperties());
	EntityPropertiesString* typeLabelQuantity = new EntityPropertiesString();
	typeLabelQuantity->setReadOnly(true);
	typeLabelQuantity->setName(_propertyDataTypeQuantity);
	typeLabelQuantity->setGroup(_groupQuantitySetttings);
	typeLabelQuantity->setValue("");
	getProperties().createProperty(typeLabelQuantity, _groupQuantitySetttings);

	EntityPropertiesSelection::createProperty(_groupQuantitySetttings, _propertyComparator, comparators, comparators.back(), "default", getProperties());
	EntityPropertiesString::createProperty(_groupQuantitySetttings, _propertyValueQuantity, "", "default", getProperties());

	//Parameter 1 settings
	EntityPropertiesSelection::createProperty(_groupParamSettings1, _propertyNameP1, {}, "", "default", getProperties());
	EntityPropertiesString* typeLabelP1 = new EntityPropertiesString();
	typeLabelP1->setReadOnly(true);
	typeLabelP1->setName(_propertyDataTypeP1);
	typeLabelP1->setGroup(_groupParamSettings1);
	typeLabelP1->setValue("");
	getProperties().createProperty(typeLabelP1, _groupParamSettings1);
	EntityPropertiesSelection::createProperty(_groupParamSettings1, _propertyComparatorP1, comparators, comparators.back(), "default", getProperties());
	EntityPropertiesString::createProperty(_groupParamSettings1, _propertyValueP1, "", "default", getProperties());

	EntityPropertiesSelection::createProperty(_groupParamSettings2, _propertyNameP2, {}, "", "default", getProperties());
	EntityPropertiesString* typeLabelP2 = new EntityPropertiesString();
	typeLabelP2->setReadOnly(true);
	typeLabelP2->setName(_propertyDataTypeP2);
	typeLabelP2->setGroup(_groupParamSettings2);
	typeLabelP2->setValue("");
	getProperties().createProperty(typeLabelP2, _groupParamSettings2);
	EntityPropertiesSelection::createProperty(_groupParamSettings2, _propertyComparatorP2, comparators, comparators.back(), "default", getProperties());
	EntityPropertiesString::createProperty(_groupParamSettings2, _propertyValueP2, "", "default", getProperties());

	EntityPropertiesSelection::createProperty(_groupParamSettings3, _propertyNameP3, {}, "", "default", getProperties());
	EntityPropertiesString* typeLabelP3 = new EntityPropertiesString();
	typeLabelP3->setReadOnly(true);
	typeLabelP3->setName(_propertyDataTypeP3);
	typeLabelP3->setGroup(_groupParamSettings3);
	typeLabelP3->setValue("");
	getProperties().createProperty(typeLabelP3, _groupParamSettings3);
	EntityPropertiesSelection::createProperty(_groupParamSettings3, _propertyComparatorP3, comparators, comparators.back(), "default", getProperties());
	EntityPropertiesString::createProperty(_groupParamSettings3, _propertyValueP3, "", "default", getProperties());

	SetVisibleParameter2(false);
	SetVisibleParameter3(false);
}

std::string EntityBlockDatabaseAccess::getSelectedProjectName()
{
	auto propertyBase = getProperties().getProperty(_propertyNameProjectName);
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
	return selectedValue == _propertyValueDimension1;
}

const bool EntityBlockDatabaseAccess::isQueryDimension2D()
{
	std::string selectedValue = getQueryDimension();
	return selectedValue == _propertyValueDimension2;
}

const bool EntityBlockDatabaseAccess::isQueryDimension3D()
{
	std::string selectedValue = getQueryDimension();
	return selectedValue == _propertyValueDimension3;
}

bool EntityBlockDatabaseAccess::SetVisibleParameter2(bool visible)
{
	const bool isVisible = getProperties().getProperty(_propertyNameP2)->getVisible();
	const bool refresh = isVisible != visible;
	if (refresh)
	{
		getProperties().getProperty(_propertyNameP2)->setVisible(visible);
		getProperties().getProperty(_propertyDataTypeP2)->setVisible(visible);
		getProperties().getProperty(_propertyComparatorP2)->setVisible(visible);
		getProperties().getProperty(_propertyValueP2)->setVisible(visible);
		this->setModified();
	}
	return refresh;
}

bool EntityBlockDatabaseAccess::SetVisibleParameter3(bool visible)
{
	bool isVisible = getProperties().getProperty(_propertyNameP3)->getVisible();
	const bool refresh = isVisible != visible;
	if (refresh)
	{
		getProperties().getProperty(_propertyNameP3)->setVisible(visible);
		getProperties().getProperty(_propertyDataTypeP3)->setVisible(visible);
		getProperties().getProperty(_propertyComparatorP3)->setVisible(visible);
		getProperties().getProperty(_propertyValueP3)->setVisible(visible);
		this->setModified();
	}
	return refresh;
}

void EntityBlockDatabaseAccess::UpdateBlockConfig()
{
	bool updateOfDimensionality = getProperties().getProperty(_propertyNameDimension)->needsUpdate();
	if (updateOfDimensionality)
	{
		const std::string queryDimension = getQueryDimension();
		std::list<std::string> connectorsForRemoval;
		if (_propertyValueDimension2 == queryDimension)
		{
			if (_connectorsByName.find(_connectorParameter3.getConnectorName()) != _connectorsByName.end())
			{
				_connectorsByName.erase(_connectorParameter3.getConnectorName());
				connectorsForRemoval.push_back(_connectorParameter3.getConnectorName());
			}
			if (_connectorsByName.find(_connectorParameter2.getConnectorName()) == _connectorsByName.end())
			{
				_connectorsByName[_connectorParameter2.getConnectorName()] = _connectorParameter2;
			}
		}
		else if (_propertyValueDimension3 == queryDimension)
		{
			if (_connectorsByName.find(_connectorParameter2.getConnectorName()) == _connectorsByName.end())
			{
				_connectorsByName[_connectorParameter2.getConnectorName()] = _connectorParameter2;
			}
			if (_connectorsByName.find(_connectorParameter3.getConnectorName()) == _connectorsByName.end())
			{
				_connectorsByName[_connectorParameter3.getConnectorName()] = _connectorParameter3;
			}
		}
		else
		{
			assert(_propertyValueDimension1 == queryDimension);
			if (_connectorsByName.find(_connectorParameter3.getConnectorName()) != _connectorsByName.end())
			{
				_connectorsByName.erase(_connectorParameter3.getConnectorName());
				connectorsForRemoval.push_back(_connectorParameter3.getConnectorName());
			}
			if (_connectorsByName.find(_connectorParameter2.getConnectorName()) != _connectorsByName.end())
			{
				_connectorsByName.erase(_connectorParameter2.getConnectorName());
				connectorsForRemoval.push_back(_connectorParameter2.getConnectorName());
			}
		}

		CreateBlockItem();
		UpdateConnections(connectorsForRemoval);
	}
}


void EntityBlockDatabaseAccess::UpdateConnections(std::list<std::string>& connectorsForRemoval)
{
	std::list<ot::UID> connectionsForRemoval;
	std::list<ot::UID> remainingConnections;
	std::map<ot::UID, EntityBase*> entityMap;
	for (auto& connectionID : _connectionIDs)
	{
		EntityBase* entityBase = readEntityFromEntityID(this, connectionID, entityMap);
		EntityBlockConnection* connectionEntity = dynamic_cast<EntityBlockConnection*>(entityBase);
		ot::GraphicsConnectionCfg connection = connectionEntity->getConnectionCfg();
		std::string connectorOfThisBlock;

		if (connection.getDestinationUid() == getEntityID())
		{
			connectorOfThisBlock = connection.destConnectable();
		}
		else
		{
			connectorOfThisBlock = connection.originConnectable();
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
	_connectionIDs = std::move(remainingConnections);
	RemoveConnectionsAtConnectedEntities(connectionsForRemoval);
}

void EntityBlockDatabaseAccess::RemoveConnectionsAtConnectedEntities(std::list<ot::UID>& connectionsForRemoval)
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
	block.setLeftTitleCornerImagePath("Images/Database.png");
	block.setBackgroundImagePath("Images/Database.svg");

	AddConnectors(block);

	auto graphicsItemConfig = block.createGraphicsItem();
	return graphicsItemConfig;
}

bool EntityBlockDatabaseAccess::updateFromProperties()
{
	auto baseProperty = getProperties().getProperty(_propertyNameDimension);
	auto selectionProperty = dynamic_cast<EntityPropertiesSelection*>(baseProperty);
	bool refresh = false;
	if (selectionProperty->getValue() == _propertyValueDimension1)
	{
		refresh = SetVisibleParameter2(false);
		refresh |= SetVisibleParameter3(false);
	}
	else if (selectionProperty->getValue() == _propertyValueDimension2)
	{
		refresh = SetVisibleParameter2(true);
		refresh |= SetVisibleParameter3(false);
	}
	else if (selectionProperty->getValue() == _propertyValueDimension3)
	{
		refresh = SetVisibleParameter2(true);
		refresh |= SetVisibleParameter3(true);
	}
	
	UpdateBlockConfig();

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

const ValueComparisionDefinition EntityBlockDatabaseAccess::getSelectedQuantityDefinition()
{
	auto baseProp = getProperties().getProperty(_propertyNameQuantity);
	auto nameProp = dynamic_cast<EntityPropertiesSelection*>(baseProp);
	const std::string name = nameProp->getValue();

	baseProp = getProperties().getProperty(_propertyValueQuantity);
	auto valueProp = dynamic_cast<EntityPropertiesString*>(baseProp);
	const std::string value = valueProp->getValue();

	baseProp = getProperties().getProperty(_propertyComparator);
	auto comparatorProp = dynamic_cast<EntityPropertiesSelection*>(baseProp);
	const std::string comparator = comparatorProp->getValue();

	baseProp = getProperties().getProperty(_propertyDataTypeQuantity);
	auto typeProp = dynamic_cast<EntityPropertiesString*>(baseProp);
	const std::string type = typeProp->getValue();

	return ValueComparisionDefinition(name, comparator, value,type);
}

const ValueComparisionDefinition EntityBlockDatabaseAccess::getSelectedParameter1Definition()
{
	auto baseProp = getProperties().getProperty(_propertyNameP1);
	auto nameProp = dynamic_cast<EntityPropertiesSelection*>(baseProp);
	const std::string name = nameProp->getValue();

	baseProp = getProperties().getProperty(_propertyValueP1);
	auto valueProp = dynamic_cast<EntityPropertiesString*>(baseProp);
	const std::string value = valueProp->getValue();

	baseProp = getProperties().getProperty(_propertyComparatorP1);
	auto comparatorProp = dynamic_cast<EntityPropertiesSelection*>(baseProp);
	const std::string comparator = comparatorProp->getValue();

	baseProp = getProperties().getProperty(_propertyDataTypeP1);
	auto typeProp = dynamic_cast<EntityPropertiesString*>(baseProp);
	const std::string type = typeProp->getValue();

	return ValueComparisionDefinition(name, comparator, value,type);
}

const ValueComparisionDefinition EntityBlockDatabaseAccess::getSelectedParameter2Definition()
{
	auto baseProp = getProperties().getProperty(_propertyNameP2);
	auto nameProp = dynamic_cast<EntityPropertiesSelection*>(baseProp);
	const std::string name = nameProp->getValue();

	baseProp = getProperties().getProperty(_propertyValueP2);
	auto valueProp = dynamic_cast<EntityPropertiesString*>(baseProp);
	const std::string value = valueProp->getValue();

	baseProp = getProperties().getProperty(_propertyComparatorP2);
	auto comparatorProp = dynamic_cast<EntityPropertiesSelection*>(baseProp);
	const std::string comparator = comparatorProp->getValue();

	baseProp = getProperties().getProperty(_propertyDataTypeP2);
	auto typeProp = dynamic_cast<EntityPropertiesString*>(baseProp);
	const std::string type = typeProp->getValue();

	return ValueComparisionDefinition(name, comparator, value, type);
}

const ValueComparisionDefinition EntityBlockDatabaseAccess::getSelectedParameter3Definition()
{
	auto baseProp = getProperties().getProperty(_propertyNameP3);
	auto nameProp = dynamic_cast<EntityPropertiesSelection*>(baseProp);
	const std::string name = nameProp->getValue();

	baseProp = getProperties().getProperty(_propertyValueP3);
	auto valueProp = dynamic_cast<EntityPropertiesString*>(baseProp);
	const std::string value = valueProp->getValue();

	baseProp = getProperties().getProperty(_propertyComparatorP3);
	auto comparatorProp = dynamic_cast<EntityPropertiesSelection*>(baseProp);
	const std::string comparator = comparatorProp->getValue();

	baseProp = getProperties().getProperty(_propertyDataTypeP3);
	auto typeProp = dynamic_cast<EntityPropertiesString*>(baseProp);
	const std::string type = typeProp->getValue();

	return ValueComparisionDefinition(name, comparator, value,type);
}