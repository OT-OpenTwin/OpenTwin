#include "EntityBlockPlot1D.h"
#include "OpenTwinCommunication/ActionTypes.h"

EntityBlockPlot1D::EntityBlockPlot1D(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	:EntityBlock(ID, parent, obs, ms, factory, owner)
{
	_navigationTreeIconName = "Plot1DVisible";
	_navigationTreeIconNameHidden = "Plot1DVisible";
	_blockTitle = "Plot 1D";

	const std::string connectorNameYAxis = "YAxis";
	const std::string connectorTitleYAxis = "Y-Axis";
	_yAxisConnector = { ot::ConnectorType::In ,connectorNameYAxis,connectorTitleYAxis };
	_connectorsByName[connectorNameYAxis] = _yAxisConnector;

	const std::string connectorNameXAxis = "XAxis";
	const std::string connectorTitleXAxis = "X-Axis";
	_xAxisConnector = { ot::ConnectorType::In ,connectorNameXAxis, connectorTitleXAxis };
	_connectorsByName[connectorNameXAxis] = _xAxisConnector;
}

void EntityBlockPlot1D::createProperties()
{
	EntityPropertiesString::createProperty("Graph properties", "X-Axis Label", "", "default", getProperties());
	EntityPropertiesString::createProperty("Graph properties", "Y-Axis Label", "", "default", getProperties());
	EntityPropertiesString::createProperty("Graph properties", "X-Axis Unit", "", "default", getProperties());
	EntityPropertiesString::createProperty("Graph properties", "Y-Axis Unit", "", "default", getProperties());
}

std::string EntityBlockPlot1D::getXLabel()
{
	auto propertyBase = getProperties().getProperty("X-Axis Label");
	auto xAxisLabel = dynamic_cast<EntityPropertiesString*>(propertyBase);
	assert(xAxisLabel != nullptr);

	return xAxisLabel->getValue();
}

std::string EntityBlockPlot1D::getYLabel()
{
	auto propertyBase = getProperties().getProperty("Y-Axis Label");
	auto yAxisLabel = dynamic_cast<EntityPropertiesString*>(propertyBase);
	assert(yAxisLabel != nullptr);

	return yAxisLabel->getValue();
}

std::string EntityBlockPlot1D::getXUnit()
{
	auto propertyBase = getProperties().getProperty("X-Axis Unit");
	auto xAxisUnit = dynamic_cast<EntityPropertiesString*>(propertyBase);
	assert(xAxisUnit != nullptr);

	return xAxisUnit->getValue();
}

std::string EntityBlockPlot1D::getYUnit()
{
	auto propertyBase = getProperties().getProperty("Y-Axis Unit");
	auto yAxisUnit = dynamic_cast<EntityPropertiesString*>(propertyBase);
	assert(yAxisUnit != nullptr);

	return yAxisUnit->getValue();
}

void EntityBlockPlot1D::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityBlock::AddStorageData(storage);
}

void EntityBlockPlot1D::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBlock::readSpecificDataFromDataBase(doc_view, entityMap);
}

ot::GraphicsItemCfg* EntityBlockPlot1D::CreateBlockCfg()
{
	std::unique_ptr<ot::GraphicsFlowItemCfg> block(new ot::GraphicsFlowItemCfg());

	const ot::Color colourTitle(ot::Color::Yellow);
	const ot::Color colourBackground(ot::Color::White);
	block->setTitleBackgroundColor(colourTitle.rInt(), colourTitle.gInt(), colourTitle.bInt());
	block->setBackgroundColor(colourBackground.rInt(), colourBackground.gInt(), colourBackground.gInt());
	//block->setLeftTitleCornerImagePath("Images/Python");

	AddConnectors(block.get());

	const std::string blockName = getClassName();
	const std::string blockTitel = CreateBlockHeadline();
	auto graphicsItemConfig = block->createGraphicsItem(blockName, blockTitel);
	return graphicsItemConfig;
}
