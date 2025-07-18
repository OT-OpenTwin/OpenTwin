#include "PipelineHandler.h"
#include "EntityBlockDatabaseAccess.h"
#include "EntityBlockPython.h"
#include "EntityBlockDisplay.h"
#include "EntityBlockFileWriter.h"
#include "EntityBlockStorage.h"

#include "BlockHandlerDatabaseAccess.h"
#include "BlockHandlerPython.h"
#include "BlockHandlerDisplay.h"
#include "BlockHandlerFileWriter.h"
#include "BlockHandlerStorage.h"

#include "OTCore/Logger.h"

void PipelineHandler::RunAll(const std::list<std::shared_ptr<GraphNode>>& rootNodes, const std::map<ot::UID, std::shared_ptr<GraphNode>>& graphNodesByBlockID, std::map<ot::UID, std::shared_ptr<EntityBlock>>& allBlockEntitiesByBlockID)
{
	try
	{
		initiate(graphNodesByBlockID, allBlockEntitiesByBlockID);
		for (std::shared_ptr<GraphNode> rootNode : rootNodes)
		{
			std::shared_ptr<BlockHandler> handler = _blockHandlerByGraphNode[rootNode];
			handler->executeOwnNode(rootNode);
		}
		_uiComponent->displayMessage("Pipeline executed successfull.\n");
	}
	catch (const std::exception& ex)
	{
		OT_LOG_E("Pipeline execution failed with error:" +  std::string(ex.what())+"\n");
	}
}

void PipelineHandler::initiate(const std::map<ot::UID, std::shared_ptr<GraphNode>>& graphNodesByBlockID, std::map<ot::UID, std::shared_ptr<EntityBlock>>& allBlockEntitiesByBlockID)
{
	
	for (auto& blockEntityByBlockID : allBlockEntitiesByBlockID)
	{
		std::shared_ptr<EntityBlock> blockEntity = blockEntityByBlockID.second;
		std::shared_ptr<GraphNode> graphNode = graphNodesByBlockID.find(blockEntity->getEntityID())->second;
		_blockHandlerByGraphNode[graphNode] = createBlockHandler(blockEntity);
		_blockHandlerByGraphNode[graphNode]->setModelComponent(_modelComponent);
		_blockHandlerByGraphNode[graphNode]->setUIComponent(_uiComponent);
	}
}

std::shared_ptr<BlockHandler> PipelineHandler::createBlockHandler(std::shared_ptr<EntityBlock> blockEntity)
{
	EntityBlockDatabaseAccess* dbAccessEntity = dynamic_cast<EntityBlockDatabaseAccess*>(blockEntity.get());
	if (dbAccessEntity != nullptr)
	{
		return std::make_shared<BlockHandlerDatabaseAccess>(dbAccessEntity, _blockHandlerByGraphNode);
	}
	
	EntityBlockPython* pythonEntity = dynamic_cast<EntityBlockPython*>(blockEntity.get());
	if (pythonEntity != nullptr)
	{
		return std::make_shared <BlockHandlerPython>(pythonEntity, _blockHandlerByGraphNode);
	}

	EntityBlockDisplay* display = dynamic_cast<EntityBlockDisplay*>(blockEntity.get());
	if (display != nullptr)
	{
		return std::make_shared<BlockHandlerDisplay>(display, _blockHandlerByGraphNode);
	}
	
	EntityBlockFileWriter* fileWriter = dynamic_cast<EntityBlockFileWriter*>(blockEntity.get());
	if (fileWriter != nullptr)
	{
		return std::make_shared<BlockHandlerFileWriter>(fileWriter, _blockHandlerByGraphNode);
	}

	EntityBlockStorage* storage = dynamic_cast<EntityBlockStorage*>(blockEntity.get());
	if (storage != nullptr)
	{
		return std::make_shared<BlockHandlerStorage>(storage, _blockHandlerByGraphNode);
	}
	
	assert(0);
	throw std::exception("Not supported block type detected.");
	
}
