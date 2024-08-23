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
	EntityPropertiesInteger::createProperty("General", "Number of parameters", 1, "default", getProperties());
	EntityPropertiesInteger::createProperty("General", "Number of quantities", 1, "default", getProperties());
	EntityPropertiesInteger::createProperty("General", "Number of meta data", 0, "default", getProperties());
}

bool EntityBlockStorage::updateFromProperties()
{
	bool refresh = false;
	EntityPropertiesBase* base = getProperties().getProperty("Number of parameters", "General");
	EntityPropertiesInteger* integerProperty = dynamic_cast<EntityPropertiesInteger*>(base);
	if (integerProperty->getValue() < 1)
	{
		integerProperty->setValue(1);
		refresh = true;
	}

	base = getProperties().getProperty("Number of quantities", "General");
	dynamic_cast<EntityPropertiesInteger*>(base);
	if (integerProperty->getValue() < 1)
	{
		integerProperty->setValue(1);
		refresh = true;
	}

	base = getProperties().getProperty("Number of meta data", "General");
	dynamic_cast<EntityPropertiesInteger*>(base);
	if (integerProperty->getValue() < 0)
	{
		integerProperty->setValue(0);
		refresh = true;
	}
	return refresh;
}
