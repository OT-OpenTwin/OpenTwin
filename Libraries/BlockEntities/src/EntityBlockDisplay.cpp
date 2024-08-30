#include "EntityBlockDisplay.h"
#include "SharedResources.h"

EntityBlockDisplay::EntityBlockDisplay(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	:EntityBlock(ID, parent, obs, ms, factory, owner)
{
	_inputConnector = { ot::ConnectorType::In, "Input", "Input"};
	_connectorsByName[_inputConnector.getConnectorName()] = _inputConnector;
	_navigationTreeIconName = BlockEntities::SharedResources::getCornerImagePath() + getIconName();
	_navigationTreeIconNameHidden = BlockEntities::SharedResources::getCornerImagePath() + getIconName();
	_blockTitle = "Display";
}

void EntityBlockDisplay::createProperties()
{
	EntityPropertiesString::createProperty("Display", "Description", "", "default", getProperties());
}

const std::string& EntityBlockDisplay::getDescription()
{
	EntityPropertiesBase* baseProperty = getProperties().getProperty("Description");
	EntityPropertiesString* stringProperty = dynamic_cast<EntityPropertiesString*>(baseProperty);
	return stringProperty->getValue();
}

ot::GraphicsItemCfg* EntityBlockDisplay::CreateBlockCfg()
{
	ot::GraphicsFlowItemBuilder block;
	block.setName(this->getClassName());
	block.setTitle(this->CreateBlockHeadline());
	
	const ot::Color colourTitle(ot::Yellow);
	block.setTitleBackgroundGradientColor(colourTitle);
	block.setLeftTitleCornerImagePath(BlockEntities::SharedResources::getCornerImagePath() + BlockEntities::SharedResources::getCornerImageNameVis());
	block.setBackgroundImagePath(BlockEntities::SharedResources::getCornerImagePath() + getIconName());
	AddConnectors(block);

	ot::GraphicsItemCfg* graphicsItemConfig = block.createGraphicsItem();
	return graphicsItemConfig;
}
