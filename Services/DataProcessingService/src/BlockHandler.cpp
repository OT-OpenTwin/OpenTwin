#include "BlockHandler.h"

BlockHandler::BlockHandler(const HandlerMap& allHandler)
	:_allHandler(allHandler)
{
}

void BlockHandler::executeOwnNode(std::shared_ptr<GraphNode> ownNode)
{
	bool proceed = executeSpecialized();
	if (proceed)
	{
		const auto& edges = ownNode->getSucceedingNodesWithEdgeInfo();
		for (auto& edge : edges)
		{
			const std::shared_ptr<GraphNode> nextNode = edge.first;
			const EdgeInfo info = edge.second;

			const std::shared_ptr<BlockHandler> nextHandler = _allHandler.find(nextNode)->second;
			nextHandler->setData(_dataPerPort[info.thisNodePort], info.succeedingNodePort);
			nextHandler->executeOwnNode(nextNode);
		}
	}
}

void BlockHandler::setData(GenericDataList& data, const std::string& targetPort)
{
	_dataPerPort[targetPort] = data;
}
