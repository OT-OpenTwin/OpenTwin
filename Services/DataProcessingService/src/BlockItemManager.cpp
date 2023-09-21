#include "BlockItemManager.h"

#include "OTGui/GraphicsCollectionCfg.h"
#include "OTGui/GraphicsPackage.h"
#include "OTGui/GraphicsLayoutItemCfg.h"

#include "BlockItemDatabaseAccess.h"
#include "BlockItemPlot1D.h"
#include "BlockItemPython.h"

#include "OpenTwinCommunication/ActionTypes.h"
#include "OpenTwinCore/Owner.h"
#include "OpenTwinCommunication/Msg.h"
#include "Application.h"
#include "OpenTwinFoundation/UiComponent.h"

#include "EntityBlockDatabaseAccess.h"
#include "OpenTwinCommunication/Msg.h"

void BlockItemManager::OrderUIToCreateBlockPicker()
{

	auto graphicsEditorPackage = BuildUpBlockPicker();
	OT_rJSON_createDOC(doc);
	OT_rJSON_createValueObject(pckgObj);
	graphicsEditorPackage->addToJsonObject(doc, pckgObj);

	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_GRAPHICSEDITOR_CreateEmptyGraphicsEditor);
	ot::rJSON::add(doc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj);
	ot::GlobalOwner::instance().addToJsonObject(doc, doc);

	std::string response;
	std::string req = ot::rJSON::toJSON(doc);
	
	auto uiComponent = Application::instance()->uiComponent();
	if (!ot::msg::send("", uiComponent->serviceURL(), ot::QUEUE, req, response)) {
		assert(0);
	}

	if (response != OT_ACTION_RETURN_VALUE_OK) {
		// Message is queued, no response here

		//OT_LOG_E("Invalid response from UI");
		//uiComponent->displayDebugMessage("Invalid response\n");
	}
}

OT_rJSON_doc BlockItemManager::CreateBlockItem(const std::string blockName, ot::UID blockID, ot::Point2DD& position)
{
	std::shared_ptr<ot::GraphicsScenePackage> block = GetBlockConfiguration(blockName, blockID, position);
	ot::GraphicsScenePackage pckg("Data Processing");
	OT_rJSON_createDOC(reqDoc);
	ot::rJSON::add(reqDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddItem);

	OT_rJSON_createValueObject(pckgDoc);
	pckg.addToJsonObject(reqDoc, pckgDoc);
	ot::rJSON::add(reqDoc, OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgDoc);

	ot::GlobalOwner::instance().addToJsonObject(reqDoc, reqDoc);
	return reqDoc;
}

ot::GraphicsItemCfg* BlockItemManager::GetBlockConfiguration(const std::string& blockName)
{

	BlockItemDatabaseAccess dbA;
	BlockItemPlot1D plot;
	BlockItemPython python;
	if (blockName == dbA.getBlockName())
	{
		return dbA.GetBlock();
	}
	else if (blockName == plot.getBlockName())
	{
		return plot.GetBlock();
	}
	else if (blockName == python.getBlockName())
	{
		return python.GetBlock();
	}
	else
	{
		throw std::exception("Block type not supported.");
	}
}

ot::GraphicsNewEditorPackage* BlockItemManager::BuildUpBlockPicker()
{
	ot::GraphicsNewEditorPackage* pckg = new ot::GraphicsNewEditorPackage(_packageName, _packageName);
	ot::GraphicsCollectionCfg* controlBlockCollection = new ot::GraphicsCollectionCfg("Control Blocks", "Control Blocks");
	ot::GraphicsCollectionCfg* controlBlockDatabaseCollection = new ot::GraphicsCollectionCfg("Database", "Database");
	ot::GraphicsCollectionCfg* controlBlockVisualizationCollection = new ot::GraphicsCollectionCfg("Visualization", "Visualization");

	ot::GraphicsCollectionCfg* mathBlockCollection = new ot::GraphicsCollectionCfg("Mathematical Operations", "Mathematical Operations");
	ot::GraphicsCollectionCfg* customizedBlockCollection = new ot::GraphicsCollectionCfg("Customized Blocks", "Customized Blocks");

	controlBlockCollection->addChildCollection(controlBlockDatabaseCollection);
	controlBlockCollection->addChildCollection(controlBlockVisualizationCollection);

	BlockItemDatabaseAccess dbA;
	BlockItemPlot1D plot;
	BlockItemPython python;

	customizedBlockCollection->addItem(python.GetBlock());
	controlBlockDatabaseCollection->addItem(dbA.GetBlock());
	controlBlockVisualizationCollection->addItem(plot.GetBlock());

	pckg->addCollection(controlBlockCollection);
	pckg->addCollection(customizedBlockCollection);
	pckg->addCollection(mathBlockCollection);

	return pckg;
}

std::shared_ptr<ot::GraphicsScenePackage> BlockItemManager::GetBlockConfiguration(const std::string blockName, ot::UID blockID, ot::Point2DD& position)
{
	ot::GraphicsScenePackage* pckg = new ot::GraphicsScenePackage(_packageName);

	BlockItemDatabaseAccess dbA;
	BlockItemPlot1D plot;
	BlockItemPython python;

	ot::GraphicsItemCfg* block = dbA.GetBlock();
	block->setPosition(position);
	block->setUid(std::to_string(blockID));
	pckg->addItem(block);
	return std::shared_ptr<ot::GraphicsScenePackage>(pckg);
}
