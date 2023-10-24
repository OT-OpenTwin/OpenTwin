#include "EntityBlockPython.h"
#include "OpenTwinCommunication/ActionTypes.h"


EntityBlockPython::EntityBlockPython(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	:EntityBlock(ID, parent, obs, ms, factory, owner), 
	_colourTitle(ot::Color::Blue), _colourBackground(ot::Color::White)
{
	_navigationTreeIconName = "python";
	_navigationTreeIconNameHidden = "python";
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
	ot::GraphicsFlowItemCfg* block = new ot::GraphicsFlowItemCfg;

	block->setTitleBackgroundColor(_colourTitle.rInt(), _colourTitle.gInt(), _colourTitle.bInt());
	block->setBackgroundColor(_colourBackground.rInt(), _colourBackground.gInt(), _colourBackground.gInt());

	block->addLeft("C0", "Parameter", ot::GraphicsFlowConnectorCfg::Square);
	block->addRight("C0", "Output", ot::GraphicsFlowConnectorCfg::Square);

	return block->createGraphicsItem("Python", "Python");
}
