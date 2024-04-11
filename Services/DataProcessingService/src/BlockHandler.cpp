#include "BlockHandler.h"

BlockHandler::BlockHandler(EntityBlock* blockEntity, const HandlerMap& allHandler)
	:_allHandler(allHandler)
{
	_blockName = blockEntity->getName();
}

BlockHandler::~BlockHandler()
{
	/*for (auto dataByName : _dataPerPort)
	{
		BlockHandler::GenericDataStructList dataList = dataByName.second;
		for (ot::GenericDataStruct* data : dataList)
		{
			if (data != nullptr)
			{
				delete data;
				data = nullptr;
			}
		}
	}*/
}

void BlockHandler::executeOwnNode(std::shared_ptr<GraphNode> ownNode)
{
	bool proceed;
	try
	{
		proceed = executeSpecialized();
	}
	catch (const std::exception& e)
	{
		const std::string message = "Execution of block " + _blockName + " failed due to the following exception: " + e.what();
		throw std::exception(message.c_str());
	}

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
