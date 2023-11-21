#include "EntityBlockPython.h"
#include "OTCommunication/ActionTypes.h"
#include "PythonHeaderInterpreter.h"

EntityBlockPython::EntityBlockPython(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	:EntityBlock(ID, parent, obs, ms, factory, owner)
{
	_navigationTreeIconName = "python";
	_navigationTreeIconNameHidden = "python";
	_blockTitle = "Python";
}

void EntityBlockPython::createProperties(const std::string& scriptFolder, ot::UID scriptFolderID)
{
	EntityPropertiesEntityList::createProperty("Script properties",_propertyNameScripts , scriptFolder, scriptFolderID, "", -1, "default", getProperties());
}

std::string EntityBlockPython::getSelectedScript()
{
	auto propBase = getProperties().getProperty(_propertyNameScripts);
	auto scriptSelection = dynamic_cast<EntityPropertiesEntityList*>(propBase);
	assert(scriptSelection != nullptr);

	return scriptSelection->getValueName();
}

ot::GraphicsItemCfg* EntityBlockPython::CreateBlockCfg()
{
	std::unique_ptr<ot::GraphicsFlowItemCfg> block(new ot::GraphicsFlowItemCfg());

	const ot::Color colourTitle(ot::Color::Cyan);
	const ot::Color colourBackground(ot::Color::White);
	block->setTitleBackgroundGradientColor(colourTitle);
	//block->setBackgroundColor(colourBackground.rInt(), colourBackground.gInt(), colourBackground.gInt());
	block->setLeftTitleCornerImagePath("Images/Python.png");
	block->setBackgroundImagePath("Images/Script.svg");

	const std::string blockName = getClassName();
	const std::string blockTitel = CreateBlockHeadline();
	AddConnectors(block.get());
	
	auto graphicsItemConfig = block->createGraphicsItem(blockName, blockTitel);
	return graphicsItemConfig;
}

bool EntityBlockPython::updateFromProperties()
{
	UpdateBlockAccordingToScriptHeader();
	CreateBlockItem();
	CreateConnections();
	return true;
}

void EntityBlockPython::UpdateBlockAccordingToScriptHeader()
{
	ResetEntity();

	auto propertyBase =	getProperties().getProperty(_propertyNameScripts);
	auto propertyEntityList = dynamic_cast<EntityPropertiesEntityList*>(propertyBase);
	ot::UID scriptID = propertyEntityList->getValueID();
	std::map<ot::UID,EntityBase*> entityMap;
	EntityBase* baseEntity = readEntityFromEntityID(nullptr, scriptID, entityMap);
	assert(baseEntity != nullptr);
	std::shared_ptr<EntityFile> scriptEntity(dynamic_cast<EntityFile*>(baseEntity));
	assert(scriptEntity != nullptr);

	PythonHeaderInterpreter headerInterpreter;
	const bool interpretationSuccessfull = headerInterpreter.interprete(scriptEntity);

	if (interpretationSuccessfull)
	{
		auto allConnectors = headerInterpreter.getAllConnectors();
		auto allProperties = headerInterpreter.getAllProperties();

		for (auto& connector : allConnectors) 
		{
			_connectorsByName[connector.getConnectorName()] = std::move(connector);
		}

	}
}

void EntityBlockPython::ResetEntity()
{
	_connectorsByName.clear();
	_connections.clear();
	
	auto allProperties = getProperties().getListOfAllProperties();
	for (auto& property : allProperties)
	{
		const std::string propertyName = property->getName();
		if (propertyName != _propertyNameScripts)
		{
			getProperties().deleteProperty(propertyName);
		}
	}
	
}

