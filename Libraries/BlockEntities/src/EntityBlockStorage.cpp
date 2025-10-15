#include "EntityBlockStorage.h"
#include "SharedResources.h"	 
#include "PropertyHelper.h"

static EntityFactoryRegistrar<EntityBlockStorage> registrar(EntityBlockStorage::className());

EntityBlockStorage::EntityBlockStorage(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner)
	:EntityBlock(ID, parent, obs, ms, owner)
{
	OldTreeIcon icon;
	icon.visibleIcon = BlockEntities::SharedResources::getCornerImagePath() + getIconName();
	icon.hiddenIcon = BlockEntities::SharedResources::getCornerImagePath() + getIconName();
	setNavigationTreeIcon(icon);

	setBlockTitle("Store in Database");

	resetModified();
}

ot::GraphicsItemCfg* EntityBlockStorage::createBlockCfg()
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

void EntityBlockStorage::createProperties()
{
	EntityPropertiesInteger::createProperty("General", "Number of inputs",1,1,m_maxNbOfInputs, "default", getProperties());
	EntityPropertiesBoolean::createProperty("General", "Create plot", false, "default", getProperties());
	EntityPropertiesString::createProperty("General", "Plot name", "", "default", getProperties());
	createConnectors();
}

bool EntityBlockStorage::updateFromProperties()
{
	// First we need to check if the number of connectors needs a refreshing
	bool requiresConnectorUpdate = getNumberOfInputs() != getConnectorCount();

	if (requiresConnectorUpdate)
	{
		clearConnectors();
		createConnectors();
		createBlockItem();
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
	for (auto& connectorByName : getAllConnectors())
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
	
	/*ot::Connector seriesMetadata(ot::ConnectorType::InOptional, getSeriesConnectorName(), "Series Metadata");
	m_connectorsByName[getSeriesConnectorName()] = seriesMetadata;
	*/
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
		addConnector(newConnector);
	}
	
}
