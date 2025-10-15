#include "EntityBlockPython.h"
#include "OTCommunication/ActionTypes.h"
#include "PythonHeaderInterpreter.h"
#include "SharedResources.h"
#include "EntityBlockHierarchicalProjectItem.h"

static EntityFactoryRegistrar<EntityBlockPython> registrar(EntityBlockPython::className());

EntityBlockPython::EntityBlockPython(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms, const std::string& owner)
	:EntityBlock(ID, parent, obs, ms, owner)
{
	m_navigationOldTreeIconName = BlockEntities::SharedResources::getCornerImagePath() + getIconName();
	m_navigationOldTreeIconNameHidden = BlockEntities::SharedResources::getCornerImagePath() + getIconName();
	m_blockTitle = "Python";
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

ot::GraphicsItemCfg* EntityBlockPython::createBlockCfg()
{
	ot::GraphicsFlowItemBuilder block;
	block.setName(this->getClassName());
	block.setTitle(this->createBlockHeadline());

	const ot::Color colourTitle(ot::Cyan);
	const ot::Color colourBackground(ot::White);
	block.setTitleBackgroundGradientColor(colourTitle);
	block.setLeftTitleCornerImagePath(BlockEntities::SharedResources::getCornerImagePath() + BlockEntities::SharedResources::getCornerImageNamePython());
	block.setBackgroundImagePath(BlockEntities::SharedResources::getCornerImagePath() + getIconName());
	//block.setBackgroundImageInsertMode(ot::GraphicsFlowItemBuilder::OnStack);

	const std::string blockName = getClassName();
	const std::string blockTitel = createBlockHeadline();
	addConnectors(block);
	
	auto graphicsItemConfig = block.createGraphicsItem();
	return graphicsItemConfig;
}

bool EntityBlockPython::updateFromProperties()
{
	auto scriptSelectionProperty =	getProperties().getProperty(_propertyNameScripts);
	if (scriptSelectionProperty->needsUpdate())
	{
		updateBlockAccordingToScriptHeader();
		createBlockItem();
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
	scriptEntity->setObserver(nullptr);

	PythonHeaderInterpreter headerInterpreter;
	const bool interpretationSuccessfull = headerInterpreter.interprete(scriptEntity);

	if (interpretationSuccessfull)
	{
		auto allConnectors = headerInterpreter.getAllConnectors();
		auto allProperties = headerInterpreter.getAllProperties();

		for (auto& connector : allConnectors) 
		{
			m_connectorsByName[connector.getConnectorName()] = std::move(connector);
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
	m_connectorsByName.clear();
	
	auto allProperties = getProperties().getListOfAllProperties();
	for (auto& property : allProperties)
	{
		const std::string propertyName = property->getName();
		const std::string propertyGroupName = property->getGroup();

		if (propertyName != _propertyNameScripts)
		{
			getProperties().deleteProperty(propertyName, propertyGroupName);
		}
	}
	
}

