#include "PipelineHandler.h"
#include "EntityBlockDatabaseAccess.h"
#include "EntityBlockPlot1D.h"
#include "EntityBlockPython.h"
#include "EntityBlockDataDimensionReducer.h"

#include "BlockHandlerDatabaseAccess.h"
#include "BlockHandlerPlot1D.h"
#include "BlockHandlerPython.h"
#include "BlockHandlerDataDimensionReducer.h"

void PipelineHandler::RunAll(const std::list<std::shared_ptr<GraphNode>>& rootNodes, const std::map<std::string, std::shared_ptr<GraphNode>>& graphNodesByBlockID, std::map<std::string, std::shared_ptr<EntityBlock>>& allBlockEntitiesByBlockID)
{
	initiate(graphNodesByBlockID,allBlockEntitiesByBlockID);
	for (std::shared_ptr<GraphNode> rootNode : rootNodes)
	{
		std::shared_ptr<BlockHandler> handler =	_blockHandlerByGraphNode[rootNode];
		handler->executeOwnNode(rootNode);
	}
}

void PipelineHandler::initiate(const std::map<std::string, std::shared_ptr<GraphNode>>& graphNodesByBlockID, std::map<std::string, std::shared_ptr<EntityBlock>>& allBlockEntitiesByBlockID)
{
	
	for (auto& blockEntityByBlockID : allBlockEntitiesByBlockID)
	{
		std::shared_ptr<EntityBlock> blockEntity = blockEntityByBlockID.second;
		std::shared_ptr<GraphNode> graphNode = graphNodesByBlockID.find(blockEntity->getBlockID())->second;
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
		return std::shared_ptr<BlockHandler>(new BlockHandlerDatabaseAccess(dbAccessEntity, _blockHandlerByGraphNode));
	}
	
	EntityBlockPlot1D* plot1DEntity = dynamic_cast<EntityBlockPlot1D*>(blockEntity.get());
	if (plot1DEntity != nullptr)
	{
		return std::shared_ptr<BlockHandler>(new BlockHandlerPlot1D(plot1DEntity, _blockHandlerByGraphNode));
	}

	EntityBlockPython* pythonEntity = dynamic_cast<EntityBlockPython*>(blockEntity.get());
	if (pythonEntity != nullptr)
	{
		return std::shared_ptr<BlockHandler>(new BlockHandlerPython(pythonEntity, _blockHandlerByGraphNode));
	}

	EntityBlockDataDimensionReducer* dataDimensionReducer = dynamic_cast<EntityBlockDataDimensionReducer*>(blockEntity.get());
	if (dataDimensionReducer != nullptr)
	{
		return std::shared_ptr<BlockHandler>(new BlockHandlerDataDimensionReducer(dataDimensionReducer,_blockHandlerByGraphNode));
	}

	throw std::exception("Not supported BlockEntity detected.");
}
