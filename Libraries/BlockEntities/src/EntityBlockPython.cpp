#include "EntityBlockPython.h"
#include "OpenTwinCommunication/ActionTypes.h"


EntityBlockPython::EntityBlockPython(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	:EntityBlock(ID, parent, obs, ms, factory, owner)
{
	_navigationTreeIconName = "python";
	_navigationTreeIconNameHidden = "python";
	_blockTitle = "Python";
}

void EntityBlockPython::createProperties(const std::string& scriptFolder, ot::UID scriptFolderID)
{
	EntityPropertiesEntityList::createProperty("Script properties", "Script", scriptFolder, scriptFolderID, "", -1, "default", getProperties());
}

std::string EntityBlockPython::getSelectedScript()
{
	auto propBase = getProperties().getProperty("Script");
	auto scriptSelection = dynamic_cast<EntityPropertiesEntityList*>(propBase);
	assert(scriptSelection != nullptr);

	return scriptSelection->getValueName();
}

ot::GraphicsItemCfg* EntityBlockPython::CreateBlockCfg()
{
	std::unique_ptr<ot::GraphicsFlowItemCfg> block(new ot::GraphicsFlowItemCfg());

	const ot::Color colourTitle(ot::Color::Cyan);
	const ot::Color colourBackground(ot::Color::White);
	block->setTitleBackgroundColor(colourTitle.rInt(), colourTitle.gInt(), colourTitle.bInt());
	block->setBackgroundColor(colourBackground.rInt(), colourBackground.gInt(), colourBackground.gInt());

	block->addLeft("C0", "Parameter", ot::GraphicsFlowConnectorCfg::Square);
	block->addRight("C1", "Output", ot::GraphicsFlowConnectorCfg::Square);

	const std::string blockName = getClassName();
	const std::string blockTitel = CreateBlockHeadline();
	auto graphicsItemConfig = block->createGraphicsItem(blockName, blockTitel);
	return graphicsItemConfig;
}
