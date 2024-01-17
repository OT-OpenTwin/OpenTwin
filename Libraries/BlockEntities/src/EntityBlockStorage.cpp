#include "EntityBlockStorage.h"

EntityBlockStorage::EntityBlockStorage(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	:EntityBlock(ID, parent, obs, ms, factory, owner)
{
	_navigationTreeIconName = "BlockDataBaseAccess";
	_navigationTreeIconNameHidden = "BlockDataBaseAccess";
	_blockTitle = "Store in Database";

	const std::string inputConnectorName = "Data";
	_dataInput = { ot::ConnectorType::In,inputConnectorName, inputConnectorName };

	_connectorsByName[inputConnectorName] = _dataInput;
}

ot::GraphicsItemCfg* EntityBlockStorage::CreateBlockCfg()
{
	ot::GraphicsFlowItemBuilder block;
	block.setName(this->getClassName());
	block.setTitle(this->CreateBlockHeadline());

	const ot::Color colourTitle(ot::Color::Lime);
	const ot::Color colourBackground(ot::Color::White);
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

}
