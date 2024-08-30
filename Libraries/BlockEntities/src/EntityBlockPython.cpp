#include "EntityBlockPython.h"
#include "OTCommunication/ActionTypes.h"
#include "PythonHeaderInterpreter.h"
#include "SharedResources.h"

EntityBlockPython::EntityBlockPython(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, ClassFactoryHandler* factory, const std::string& owner)
	:EntityBlock(ID, parent, obs, ms, factory, owner)
{
	_navigationTreeIconName = BlockEntities::SharedResources::getCornerImagePath() + getIconName();
	_navigationTreeIconNameHidden = BlockEntities::SharedResources::getCornerImagePath() + getIconName();
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
	ot::GraphicsFlowItemBuilder block;
	block.setName(this->getClassName());
	block.setTitle(this->CreateBlockHeadline());

	const ot::Color colourTitle(ot::Cyan);
	const ot::Color colourBackground(ot::White);
	block.setTitleBackgroundGradientColor(colourTitle);
	block.setLeftTitleCornerImagePath(BlockEntities::SharedResources::getCornerImagePath() + BlockEntities::SharedResources::getCornerImageNamePython());
	block.setBackgroundImagePath(BlockEntities::SharedResources::getCornerImagePath() + getIconName());
	//block.setBackgroundImageInsertMode(ot::GraphicsFlowItemBuilder::OnStack);

	const std::string blockName = getClassName();
	const std::string blockTitel = CreateBlockHeadline();
	AddConnectors(block);
	
	auto graphicsItemConfig = block.createGraphicsItem();
	return graphicsItemConfig;
}

bool EntityBlockPython::updateFromProperties()
{
	auto scriptSelectionProperty =	getProperties().getProperty(_propertyNameScripts);
	if (scriptSelectionProperty->needsUpdate())
	{
		updateBlockAccordingToScriptHeader();
		CreateBlockItem();
	}
	getProperties().forceResetUpdateForAllProperties();
	return true;
}

void EntityBlockPython::updateBlockAccordingToScriptHeader()
{
	resetBlockRelatedAttributes();

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

		for (auto& property : allProperties)
		{
			getProperties().createProperty(property, property->getGroup());
		}
	}
	else
	{
		std::string report = headerInterpreter.getErrorReport();
		report = "\nError while analysing pythonscript:\n" + report;
		ot::JsonDocument cmdDoc;
		cmdDoc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UI_DisplayMessage,cmdDoc.GetAllocator());
		auto jReport =	ot::JsonString(report, cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_MESSAGE, jReport, cmdDoc.GetAllocator());
		getObserver()->sendMessageToViewer(cmdDoc);
	}
}

void EntityBlockPython::resetBlockRelatedAttributes()
{
	_connectorsByName.clear();
	_connectionIDs.clear();
	
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

