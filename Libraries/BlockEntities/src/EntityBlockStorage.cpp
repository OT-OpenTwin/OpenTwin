#include "EntityBlockStorage.h"
#include "SharedResources.h"	 

EntityBlockStorage::EntityBlockStorage(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	:EntityBlock(ID, parent, obs, ms, factory, owner)
{
	_navigationTreeIconName = BlockEntities::SharedResources::getCornerImagePath() + getIconName();
	_navigationTreeIconNameHidden = BlockEntities::SharedResources::getCornerImagePath() + getIconName();
	_blockTitle = "Store in Database";
}

ot::GraphicsItemCfg* EntityBlockStorage::CreateBlockCfg()
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

void EntityBlockStorage::createProperties()
{
	EntityPropertiesString::createProperty("Series Metadata", "Name", "Series", "default", getProperties());

	EntityPropertiesInteger::createProperty(m_groupGeneral, m_propertyNbOfQuantities, 1, "default", getProperties());
	EntityPropertiesString::createProperty(m_quantityInputNameBase, m_propertyName, "", "default", getProperties());
	EntityPropertiesString::createProperty(m_quantityInputNameBase, m_propertyType, "", "default", getProperties());
	EntityPropertiesString::createProperty(m_quantityInputNameBase, m_propertyUnit, "", "default", getProperties());

	EntityPropertiesInteger::createProperty(m_groupGeneral, m_propertyNbOfParameter, 1, "default", getProperties());
	EntityPropertiesBoolean::createProperty(m_parameterInputNameBase, m_propertyConstParameter, false, "default", getProperties());
	EntityPropertiesString::createProperty(m_parameterInputNameBase, m_propertyName, "", "default", getProperties());
	EntityPropertiesString::createProperty(m_parameterInputNameBase, m_propertyUnit, "", "default", getProperties());
	
	EntityPropertiesInteger::createProperty(m_groupGeneral, m_propertyNbOfMetaData, 0, "default", getProperties());
	createConnectors();
}

bool EntityBlockStorage::updateFromProperties()
{
	//First we need to check if the number of connectors needs a refreshing
	bool requiresConnectorUpdate = false;

	//Checking parameter connectors
	EntityPropertiesBase* base = getProperties().getProperty(m_propertyNbOfParameter, m_groupGeneral);
	EntityPropertiesInteger* integerProperty = dynamic_cast<EntityPropertiesInteger*>(base);

	if (integerProperty->getValue() < 1)
	{
		integerProperty->setValue(1);
	}
	const int64_t numberOfParameter = integerProperty->getValue();
	if (numberOfParameter != m_parameterInputs.size())
	{
		requiresConnectorUpdate = true;
	}

	//Checking quantities connectors
	base = getProperties().getProperty(m_propertyNbOfQuantities, m_groupGeneral);
	integerProperty = dynamic_cast<EntityPropertiesInteger*>(base);
	if (integerProperty->getValue() < 1)
	{
		integerProperty->setValue(1);
	}
	const int64_t numberOfQuantities = integerProperty->getValue();
	if (numberOfQuantities != m_quantityInputs.size())
	{
		requiresConnectorUpdate = true;
	}

	//Checking meta data connectors
	base = getProperties().getProperty(m_propertyNbOfMetaData, m_groupGeneral);
	integerProperty = dynamic_cast<EntityPropertiesInteger*>(base);
	if (integerProperty->getValue() < 0)
	{
		integerProperty->setValue(0);
	}
	const int64_t numberOfMetadata = integerProperty->getValue();
	if (integerProperty->getValue() != m_metaDataInputs.size())
	{
		requiresConnectorUpdate = true;
	}

	//Next we check the properties associated with the parameters, quantities and meta data
	std::list<std::string> propertiesForDeletion;
	std::list<EntityPropertiesBase*> allProperties = getProperties().getListOfAllProperties();
	
	//Checking if the number of parameter, quantities or meta data was reduced. Deleting the properties that are too much.
	for (EntityPropertiesBase* aProperty : allProperties)
	{
		const std::string groupName = aProperty->getGroup();
		const bool isParameterProperty = groupName.find(m_parameterInputNameBase) != std::string::npos;
		if (isParameterProperty && m_parameterInputNameBase != groupName)
		{
			const std::string paramIndex = groupName.substr(m_parameterInputNameBase.size(), groupName.size());
			const int32_t index = std::stoi(paramIndex);
			//parameter(numberOfParameter=1),parameter 1(numberOfParameter=2), parameter 2(numberOfParameter=3)
			if (index > numberOfParameter - 1)
			{
				propertiesForDeletion.push_back(groupName);
			}
		}

		const bool isQuantityProperty = groupName.find(m_quantityInputNameBase) != std::string::npos;
		if (isQuantityProperty && m_quantityInputNameBase != groupName)
		{
			const std::string paramIndex = groupName.substr(m_quantityInputNameBase.size(), groupName.size());
			const int32_t index = std::stoi(paramIndex);
			//parameter(numberOfParameter=1),parameter 1(numberOfParameter=2), parameter 2(numberOfParameter=3)
			if (index > numberOfQuantities - 1)
			{
				propertiesForDeletion.push_back(groupName);
			}
		}

		const bool isMetadataProperty = groupName.find(m_metaDataInputNameBase) != std::string::npos;
		if (isMetadataProperty && m_metaDataInputNameBase != groupName)
		{
			const std::string paramIndex = groupName.substr(m_metaDataInputNameBase.size(), groupName.size());
			const int32_t index = std::stoi(paramIndex);
			//parameter(numberOfParameter=1),parameter 1(numberOfParameter=2), parameter 2(numberOfParameter=3)
			if (index > numberOfMetadata - 1)
			{
				propertiesForDeletion.push_back(groupName);
			}
		}
	}
	propertiesForDeletion.unique();
	for (std::string& groupOfPropertyForDeletion : propertiesForDeletion)
	{
		getProperties().deleteProperty(m_propertyName, groupOfPropertyForDeletion);
		getProperties().deleteProperty(m_propertyUnit, groupOfPropertyForDeletion);
		getProperties().deleteProperty(m_propertyType, groupOfPropertyForDeletion);
		getProperties().deleteProperty(m_propertyConstParameter, groupOfPropertyForDeletion);
	}

	//Next we check if additional properties are needed
	for (int32_t index = 1; index < numberOfParameter; index++)
	{
		const std::string groupNameParameter = m_parameterInputNameBase + " " + std::to_string(index);
		const bool parameterPropertiesNotExisting = getProperties().getListOfPropertiesForGroup(groupNameParameter).empty();
		if (parameterPropertiesNotExisting)
		{
			EntityPropertiesBoolean::createProperty(groupNameParameter, m_propertyConstParameter, false, "default", getProperties());
			EntityPropertiesString::createProperty(groupNameParameter, m_propertyName, "", "default", getProperties());
			EntityPropertiesString::createProperty(groupNameParameter, m_propertyUnit, "", "default", getProperties());
		}
	}
	for (int32_t index = 1; index < numberOfQuantities; index++)
	{
		const std::string groupNameQuantity = m_quantityInputNameBase + " " + std::to_string(index);
		const bool quantityPropertiesNotExisting = getProperties().getListOfPropertiesForGroup(groupNameQuantity).empty();
		if (quantityPropertiesNotExisting)
		{
			EntityPropertiesString::createProperty(groupNameQuantity, m_propertyName, "", "default", getProperties());
			EntityPropertiesString::createProperty(groupNameQuantity, m_propertyUnit, "", "default", getProperties());
			EntityPropertiesString::createProperty(groupNameQuantity, m_propertyType, "", "default", getProperties());
		}
	}
	for(int32_t index = 1; index <= numberOfMetadata; index++)
	{
		std::string groupNameMetaData = m_metaDataInputNameBase  + " " + std::to_string(index);
		const bool metaDataPropertiesNotExisting = getProperties().getListOfPropertiesForGroup(groupNameMetaData).empty();
		if (metaDataPropertiesNotExisting)
		{
			EntityPropertiesString::createProperty(groupNameMetaData, m_propertyName, "", "default", getProperties());
		}
	}


	if (requiresConnectorUpdate)
	{
		clearConnectors();
		createConnectors();
		CreateBlockItem();
	}

	return true;
}

int32_t EntityBlockStorage::getNumberOfQuantities()
{
	EntityPropertiesBase* base = getProperties().getProperty(m_propertyNbOfQuantities, m_groupGeneral);
	assert(base != nullptr);
	EntityPropertiesInteger* integerProp = dynamic_cast<EntityPropertiesInteger*>(base);
	assert(integerProp != nullptr);
	return integerProp->getValue();
}

int32_t EntityBlockStorage::getNumberOfParameters()
{
	EntityPropertiesBase* base = getProperties().getProperty(m_propertyNbOfParameter, m_groupGeneral);
	assert(base != nullptr);
	EntityPropertiesInteger* integerProp = dynamic_cast<EntityPropertiesInteger*>(base);
	assert(integerProp != nullptr);
	return integerProp->getValue();
}

int32_t EntityBlockStorage::getNumberOfMetaData()
{
	EntityPropertiesBase* base = getProperties().getProperty(m_propertyNbOfMetaData, m_groupGeneral);
	assert(base != nullptr);
	EntityPropertiesInteger* integerProp = dynamic_cast<EntityPropertiesInteger*>(base);
	assert(integerProp != nullptr);
	return integerProp->getValue();
}

const std::list<ot::Connector> EntityBlockStorage::getConnectorsQuantity()
{
	std::list<ot::Connector> allQuantityConnectors;
	for (auto connectorByName : _connectorsByName)
	{
		const std::string connectorName = connectorByName.first;
		if (connectorName.find(m_quantityInputNameBase) != std::string::npos)
		{
			allQuantityConnectors.push_back(connectorByName.second);
		}
	}
	return allQuantityConnectors;
}

const std::list<ot::Connector> EntityBlockStorage::getConnectorsParameter()
{
	std::list<ot::Connector> allParameterConnectors;
	for (auto connectorByName : _connectorsByName)
	{
		const std::string connectorName = connectorByName.first;
		if (connectorName.find(m_parameterInputNameBase) != std::string::npos)
		{
			allParameterConnectors.push_back(connectorByName.second);
		}
	}
	return allParameterConnectors;
}

const std::list<ot::Connector> EntityBlockStorage::getConnectorsMetadata()
{
	std::list<ot::Connector> allMetadataConnectors;
	for (auto connectorByName : _connectorsByName)
	{
		const std::string connectorName = connectorByName.first;
		if (connectorName.find(m_metaDataInputNameBase) != std::string::npos)
		{
			allMetadataConnectors.push_back(connectorByName.second);
		}
	}
	return allMetadataConnectors;
}

const ParameterProperties EntityBlockStorage::getPropertiesParameter(const std::string& _parameterName)
{
	ParameterProperties properties;
	
	EntityPropertiesBase* base = getProperties().getProperty(m_propertyConstParameter, _parameterName);
	assert(base != nullptr);
	EntityPropertiesBoolean* boolProperty = dynamic_cast<EntityPropertiesBoolean*>(base);
	assert(boolProperty != nullptr);
	properties.m_propertyConstParameter = boolProperty->getValue();
	
	base = getProperties().getProperty(m_propertyName, _parameterName);
	assert(base != nullptr);
	EntityPropertiesString* stringProperty = dynamic_cast<EntityPropertiesString*>(base);
	assert(stringProperty != nullptr);
	properties.m_propertyName =	stringProperty->getValue();

	base = getProperties().getProperty(m_propertyUnit, _parameterName);
	assert(base != nullptr);
	stringProperty = dynamic_cast<EntityPropertiesString*>(base);
	assert(stringProperty != nullptr);
	properties.m_propertyUnit = stringProperty->getValue();

	return properties;
}

const QuantityProperties EntityBlockStorage::getPropertiesQuantity(const std::string& _quantityName)
{
	QuantityProperties quantityProperties;

	EntityPropertiesBase* base = getProperties().getProperty(m_propertyName, _quantityName);
	EntityPropertiesString* stringProperty = dynamic_cast<EntityPropertiesString*>(base);
	assert(stringProperty != nullptr);
	quantityProperties.m_propertyName = stringProperty->getValue();

	base = getProperties().getProperty(m_propertyType, _quantityName);
	stringProperty = dynamic_cast<EntityPropertiesString*>(base);
	assert(stringProperty != nullptr);
	quantityProperties.m_propertyType = stringProperty->getValue();

	base = getProperties().getProperty(m_propertyUnit, _quantityName);
	stringProperty = dynamic_cast<EntityPropertiesString*>(base);
	assert(stringProperty != nullptr);
	quantityProperties.m_propertyUnit = stringProperty->getValue();

	return quantityProperties;
}

std::string EntityBlockStorage::getSeriesName()
{
	EntityPropertiesBase* base = getProperties().getProperty("Name", "Series Metadata");
	assert(base != nullptr);
	EntityPropertiesString* stringProp = dynamic_cast<EntityPropertiesString*>(base);
	assert(stringProp != nullptr);
	return stringProp->getValue();
}

void EntityBlockStorage::createParameterProperties(const std::string& _groupName)
{
	EntityPropertiesBoolean::createProperty(_groupName, "Constant for entire dataset",false,"default",getProperties());
}

void EntityBlockStorage::createConnectors()
{
	const ot::ConnectorType type = ot::ConnectorType::In;
	
	int32_t numberOfQuantities = getNumberOfQuantities();
	for (int32_t i = 1; i <= numberOfQuantities;i++)
	{
		std::string name = m_quantityInputNameBase;
		std::string title = m_quantityInputNameBase;
		if (i > 1)
		{
			title += " " + std::to_string(i - 1);
			name += std::to_string(i - 1);
		}
		ot::Connector newConnector(type, name, title);
		_connectorsByName[name] = newConnector;
		m_quantityInputs.push_back(&_connectorsByName.find(name)->second);
	}
	
	int32_t numberOfParameter = getNumberOfParameters();
	for (int32_t i = 1; i <= numberOfParameter;i++)
	{
		std::string name = m_parameterInputNameBase;
		std::string title = m_parameterInputNameBase;
		if (i > 1)
		{
			title += " " + std::to_string(i - 1);
			name += std::to_string(i - 1);
		}
		ot::Connector newConnector(type, name, title);
		_connectorsByName[name] = newConnector;
		m_parameterInputs.push_back(&_connectorsByName.find(name)->second);
	}
	
	int32_t numberOfMetaData = getNumberOfMetaData();
	for (int32_t i = 1; i <= numberOfMetaData; i++)
	{
		std::string name = m_metaDataInputNameBase;
		std::string title = m_metaDataInputNameBase;
		title += " " + std::to_string(i);
		name += std::to_string(i);
		
		ot::Connector newConnector(type, name, title);
		_connectorsByName[name] = newConnector;
		m_metaDataInputs.push_back(&_connectorsByName.find(name)->second);
	}

}

void EntityBlockStorage::clearConnectors()
{
	m_metaDataInputs.clear();
	m_parameterInputs.clear();
	m_quantityInputs.clear();
	_connectorsByName.clear();
}
