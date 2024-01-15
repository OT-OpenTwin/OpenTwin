#include "EntityBlockDataDimensionReducer.h"

EntityBlockDataDimensionReducer::EntityBlockDataDimensionReducer(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	:EntityBlock(ID,parent,obs,ms,factory,owner)
{
	_navigationTreeIconName = "BlockDataBaseAccess";
	_navigationTreeIconNameHidden = "BlockDataBaseAccess";
	_blockTitle = "Matrix Element Selection";

	const std::string inputConnectorName = "In";
	_inputConnector = { ot::ConnectorType::In,inputConnectorName, inputConnectorName };
	const std::string outputConnectorName = "Out";
	_outputConnector = { ot::ConnectorType::Out,outputConnectorName, outputConnectorName};

	_connectorsByName[outputConnectorName] = _outputConnector;
	_connectorsByName[inputConnectorName] = _inputConnector;
}

ot::GraphicsItemCfg* EntityBlockDataDimensionReducer::CreateBlockCfg()
{
	ot::GraphicsFlowItemBuilder block;
	block.setName(this->getClassName());
	block.setTitle(this->CreateBlockHeadline());

	const ot::Color colourTitle(ot::Color::Lime);
	const ot::Color colourBackground(ot::Color::White);
	block.setTitleBackgroundGradientColor(colourTitle);
	block.setLeftTitleCornerImagePath("Images/Database.png");
	
	AddConnectors(block);

	auto graphicsItemConfig = block.createGraphicsItem();
	return graphicsItemConfig;
}

void EntityBlockDataDimensionReducer::createProperties()
{
	EntityPropertiesSelection::createProperty("Output Dimensions", "Output Dimensions", { "1", "2", "3" }, "1", "default", getProperties());
	EntityPropertiesInteger::createProperty("Input Index", "Column", 0, "default", getProperties());
	EntityPropertiesInteger::createProperty("Input Index", "Row", 0, "default", getProperties());
}

int EntityBlockDataDimensionReducer::getInputRow()
{
	auto baseProperty = getProperties().getProperty("Row");
	auto integerProperty = dynamic_cast<EntityPropertiesInteger*>(baseProperty);
	return integerProperty->getValue();
}

int EntityBlockDataDimensionReducer::getInputColumn()
{
	auto baseProperty =	getProperties().getProperty("Column");
	auto integerProperty =	dynamic_cast<EntityPropertiesInteger*>(baseProperty);
	return integerProperty->getValue();
}
