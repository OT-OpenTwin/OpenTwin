#include "EntityBlockStorage.h"
#include "SharedResources.h"	 
#include "PropertyHelper.h"

EntityBlockStorage::EntityBlockStorage(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	:EntityBlock(ID, parent, obs, ms, factory, owner)
{
	m_navigationOldTreeIconName = BlockEntities::SharedResources::getCornerImagePath() + getIconName();
	m_navigationOldTreeIconNameHidden = BlockEntities::SharedResources::getCornerImagePath() + getIconName();
	m_blockTitle = "Store in Database";
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
	EntityPropertiesInteger::createProperty("General", "Number of inputs",1,1,m_maxNbOfInputs, "default", getProperties());
	EntityPropertiesBoolean::createProperty("General", "Create plot", false, "default", getProperties());
	EntityPropertiesString::createProperty("General", "Plot name", "", "default", getProperties());
	createConnectors();
}

bool EntityBlockStorage::updateFromProperties()
{
	//First we need to check if the number of connectors needs a refreshing
	bool requiresConnectorUpdate = getNumberOfInputs() != m_connectorsByName.size();

	if (requiresConnectorUpdate)
	{
		clearConnectors();
		createConnectors();
		CreateBlockItem();
	}

	return true;
}

uint32_t EntityBlockStorage::getNumberOfInputs()
{
	int32_t nbOfInputs =	PropertyHelper::getIntegerPropertyValue(this, "Number of inputs", "General");
	return static_cast<uint32_t>(nbOfInputs);
}

bool EntityBlockStorage::getCreatePlot()
{
	return PropertyHelper::getBoolPropertyValue(this, "Create plot", "General");
}

std::string EntityBlockStorage::getPlotName()
{
	return PropertyHelper::getStringPropertyValue(this, "Plot name", "General");
}

const std::list<std::string> EntityBlockStorage::getInputNames()
{
	std::list<std::string> inputNames;
	for (auto& connectorByName : m_connectorsByName)
	{
		if (connectorByName.first != getSeriesConnectorName())
		{
			inputNames.push_back(connectorByName.first);
		}
	}
	return inputNames;
}

void EntityBlockStorage::createParameterProperties(const std::string& _groupName)
{
	EntityPropertiesBoolean::createProperty(_groupName, "Constant for entire dataset",false,"default",getProperties());
}

void EntityBlockStorage::createConnectors()
{
	const ot::ConnectorType type = ot::ConnectorType::In;
	
	ot::Connector seriesMetadata(ot::ConnectorType::InOptional, getSeriesConnectorName(), "Series Metadata");
	m_connectorsByName[getSeriesConnectorName()] = seriesMetadata;
	
	int32_t numberOfInputs = getNumberOfInputs();
	for (int32_t i = 1; i <= numberOfInputs;i++)
	{
		std::string name = "Input";
		std::string title = "Data Input";
		if (i > 1)
		{
			title += " " + std::to_string(i -1);
			name += std::to_string(i -1);
		}
		ot::Connector newConnector(type, name, title);
		m_connectorsByName[name] = newConnector;
	}
	
}

void EntityBlockStorage::clearConnectors()
{
	m_connectorsByName.clear();
}
