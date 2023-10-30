#pragma once
#include <list>
#include "OpenTwinCore/Variable.h"
#include "OpenTwinFoundation/BusinessLogicHandler.h"
#include "GraphNode.h"


using genericDataBlock = std::list<ot::Variable>;

class BlockHandler : public BusinessLogicHandler
{
public:
	using HandlerMap = std::map<std::shared_ptr<GraphNode>, std::shared_ptr<BlockHandler>>;
	BlockHandler(const HandlerMap& allHandler) :_allHandler(allHandler) {};
	void executeOwnNode(std::shared_ptr<GraphNode> ownNode)
	{
		executeSpecialized();
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

	virtual void setData(genericDataBlock& data, const std::string& targetPort) = 0;	

protected:
	virtual void executeSpecialized() = 0;
	std::map<std::string, genericDataBlock> _dataPerPort;
private:
	const HandlerMap& _allHandler;
};
