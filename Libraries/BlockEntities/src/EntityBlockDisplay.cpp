// @otlicense

#include "EntityBlockDisplay.h"
#include "SharedResources.h"

static EntityFactoryRegistrar<EntityBlockDisplay> registrar(EntityBlockDisplay::className());

EntityBlockDisplay::EntityBlockDisplay(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner)
	:EntityBlock(ID, parent, obs, ms, owner)
{
	OldTreeIcon icon;
	icon.visibleIcon = BlockEntities::SharedResources::getCornerImagePath() + getIconName();
	icon.hiddenIcon = BlockEntities::SharedResources::getCornerImagePath() + getIconName();
	setNavigationTreeIcon(icon);

	setBlockTitle("Display");

	_inputConnector = { ot::ConnectorType::In, "Input", "Input"};
	addConnector(_inputConnector);

	resetModified();
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

ot::GraphicsItemCfg* EntityBlockDisplay::createBlockCfg()
{
	ot::GraphicsFlowItemBuilder block;
	block.setName(this->getClassName());
	block.setTitle(this->createBlockHeadline());
	
	const ot::Color colourTitle(ot::Yellow);
	block.setTitleBackgroundGradientColor(colourTitle);
	block.setLeftTitleCornerImagePath(BlockEntities::SharedResources::getCornerImagePath() + BlockEntities::SharedResources::getCornerImageNameVis());
	block.setBackgroundImagePath(BlockEntities::SharedResources::getCornerImagePath() + getIconName());
	addConnectors(block);

	ot::GraphicsItemCfg* graphicsItemConfig = block.createGraphicsItem();
	return graphicsItemConfig;
}
