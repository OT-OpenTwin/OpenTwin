#pragma once
#include <list>

#include "EntityBlock.h"
#include "OpenTwinFoundation/BusinessLogicHandler.h"
#include "GraphNode.h"
#include "BlockHandler.h"
//#include "Pipeline.h"
//
class PipelineHandler : public BusinessLogicHandler
{
public:
	void RunAll(const std::list < std::shared_ptr<GraphNode>>& rootNodes, const std::map<std::string, std::shared_ptr<GraphNode>>& graphNodesByBlockID, std::map<std::string, std::shared_ptr<EntityBlock>>& allBlockEntitiesByBlockID);

private:
	std::map<std::shared_ptr<GraphNode>, std::shared_ptr<BlockHandler>> _blockHandlerByGraphNode;

	void initiate(const std::map<std::string, std::shared_ptr<GraphNode>>& graphNodesByBlockID, std::map<std::string, std::shared_ptr<EntityBlock>>& allBlockEntitiesByBlockID);
	std::shared_ptr<BlockHandler> createBlockHandler(std::shared_ptr<EntityBlock> blockEntity);
};
