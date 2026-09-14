#include "OTBlockEntities/Pipeline/EntityBlockRefinement.h"
#include "OTBlockEntities/BlockImageNames.h"
#include "OTModelEntities/Properties/PropertyHelper.h"
#include "OTCore/ProductMasterData/ZoneTag.h"

static EntityFactoryRegistrar<EntityBlockRefinement> registrar(EntityBlockRefinement::className());

EntityBlockRefinement::EntityBlockRefinement(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms)
	: ot::EntityBlockPipelineBase(ID, parent, obs, ms)
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon(ot::BlockImageNames::getCornerImagePath() + getIconName());
	treeItem.setHiddenIcon(ot::BlockImageNames::getCornerImagePath() + getIconName());
	this->setDefaultTreeItem(treeItem);

	setBlockTitle("Data Lake Zone Access");

	m_outputConnector = { ot::ConnectorType::Out, "Entities in zone", "Entities in zone" };
	addConnector(m_outputConnector);
}

ot::GraphicsItemCfg* EntityBlockRefinement::createBlockCfg()
{
	ot::GraphicsFlowItemBuilder block;
	block.setName(this->getClassName());
	block.setTitle(this->createBlockHeadline());

	const ot::Color colourTitle(ot::DarkOrange);
	block.setTitleBackgroundGradientColor(colourTitle);
	block.setLeftTitleCornerImagePath(ot::BlockImageNames::getCornerImagePath() + ot::BlockImageNames::getCornerImageNameDLZone());
	block.setBackgroundImagePath(ot::BlockImageNames::getCornerImagePath() + getIconName());

	addConnectors(block);

	ot::GraphicsItemCfg* graphicsItemConfig = block.createGraphicsItem();
	return graphicsItemConfig;
}

void EntityBlockRefinement::createProperties()
{
	ot::ZoneTagConverter converter;
	std::list<std::string> zoneTags = converter.getAllZoneTags();
	EntityPropertiesSelection::createProperty("Data lake zone", "Zone tag", zoneTags, *zoneTags.begin(), "default", getProperties());
}

std::string EntityBlockRefinement::getSelectedZone()
{
	const std::string selectedZone = PropertyHelper::getSelectionPropertyValue(this, "Zone tag", "Data lake zone");
	return selectedZone;
}
