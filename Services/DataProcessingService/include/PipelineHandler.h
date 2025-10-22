#pragma once
#include <list>

#include "EntityBlock.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "GraphNode.h"
#include "BlockHandler.h"
#include "SolverReport.h"


class PipelineHandler : public BusinessLogicHandler
{
public:
	void runAll(const std::list < std::shared_ptr<GraphNode>>& _rootNodes, const std::map<ot::UID, std::shared_ptr<GraphNode>>& _graphNodesByBlockID, std::map<ot::UID, std::shared_ptr<EntityBlock>>& _allBlockEntitiesByBlockID);
	void setSolverName(const std::string& _solverName)
	{
		SolverReport::instance().setSolverName(_solverName);
	}

private:
	std::map<std::shared_ptr<GraphNode>, std::shared_ptr<BlockHandler>> m_blockHandlerByGraphNode;

	void initiate(const std::map<ot::UID, std::shared_ptr<GraphNode>>& _graphNodesByBlockID, std::map<ot::UID, std::shared_ptr<EntityBlock>>& _allBlockEntitiesByBlockID);
	std::shared_ptr<BlockHandler> createBlockHandler(std::shared_ptr<EntityBlock> _blockEntity);
};
