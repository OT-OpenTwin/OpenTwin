#include "EntityBlockStorage.h"

EntityBlockStorage::EntityBlockStorage(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	:EntityBlock(ID, parent, obs, ms, factory, owner)
{
	_navigationTreeIconName = "BlockDataBaseAccess";
	_navigationTreeIconNameHidden = "BlockDataBaseAccess";
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
	block.setLeftTitleCornerImagePath("Images/Database.png");
	block.setBackgroundImagePath("Images/Database.svg");

	AddConnectors(block);

	auto graphicsItemConfig = block.createGraphicsItem();
	return graphicsItemConfig;
}

void EntityBlockStorage::createProperties()
{
	EntityPropertiesString::createProperty("Series Metadata", "Name", "Series", "default", getProperties());
	EntityPropertiesInteger::createProperty(m_groupGeneral, m_propertyNbOfQuantities, 1, "default", getProperties());
	EntityPropertiesInteger::createProperty(m_groupGeneral, m_propertyNbOfParameter, 1, "default", getProperties());
	EntityPropertiesInteger::createProperty(m_groupGeneral, m_propertyNbOfMetaData, 0, "default", getProperties());
	createConnectors();
}

bool EntityBlockStorage::updateFromProperties()
{
	bool requiresConnectorUpdate = false;
	
	EntityPropertiesBase* base = getProperties().getProperty(m_propertyNbOfParameter, m_groupGeneral);
	EntityPropertiesInteger* integerProperty = dynamic_cast<EntityPropertiesInteger*>(base);
	
	if (integerProperty->getValue() < 1)
	{
		integerProperty->setValue(1);
	}
	if (integerProperty->getValue() != m_parameterInputs.size())
	{
		requiresConnectorUpdate = true;
	}

	base = getProperties().getProperty(m_propertyNbOfQuantities, m_groupGeneral);
	integerProperty = dynamic_cast<EntityPropertiesInteger*>(base);
	if (integerProperty->getValue() < 1)
	{
		integerProperty->setValue(1);
	}
	if (integerProperty->getValue() != m_quantityInputs.size())
	{
		requiresConnectorUpdate = true;
	}

	base = getProperties().getProperty(m_propertyNbOfMetaData, m_groupGeneral);
	integerProperty = dynamic_cast<EntityPropertiesInteger*>(base);
	if (integerProperty->getValue() < 0)
	{
		integerProperty->setValue(0);
	}
	if (integerProperty->getValue() != m_metaDataInputs.size())
	{
		requiresConnectorUpdate = true;
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
		if (i > 1)
		{
			title += " " + std::to_string(i - 1);
			name += std::to_string(i - 1);
		}
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
