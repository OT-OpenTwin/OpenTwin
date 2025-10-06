#pragma once
#include <list>

#include "EntityBlock.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "GraphNode.h"
#include "BlockHandler.h"
//#include "Pipeline.h"


class PipelineHandler : public BusinessLogicHandler
{
public:
	void RunAll(const std::list < std::shared_ptr<GraphNode>>& rootNodes, const std::map<ot::UID, std::shared_ptr<GraphNode>>& graphNodesByBlockID, std::map<ot::UID, std::shared_ptr<EntityBlock>>& allBlockEntitiesByBlockID);
	void setSolverName(const std::string& _solverName)
	{
		std::string& g_solverName = getSolverName();
		g_solverName = _solverName;
	}

	static std::string& getSolverName() 
	{
		static thread_local std::string g_solverName;
		return g_solverName; 
	}

private:
	std::map<std::shared_ptr<GraphNode>, std::shared_ptr<BlockHandler>> _blockHandlerByGraphNode;

	void initiate(const std::map<ot::UID, std::shared_ptr<GraphNode>>& graphNodesByBlockID, std::map<ot::UID, std::shared_ptr<EntityBlock>>& allBlockEntitiesByBlockID);
	std::shared_ptr<BlockHandler> createBlockHandler(std::shared_ptr<EntityBlock> blockEntity);
};
