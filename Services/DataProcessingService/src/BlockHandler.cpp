// @otlicense

#include "BlockHandler.h"
#include "OTServiceFoundation/TimeFormatter.h"
#include "SolverReport.h"
BlockHandler::BlockHandler(EntityBlock* blockEntity, const HandlerMap& allHandler)
	:m_allHandler(allHandler)
{
	m_blockName = blockEntity->getName();
}

void BlockHandler::executeOwnNode(std::shared_ptr<GraphNode> ownNode)
{
	bool proceed;
	try
	{
		SolverReport::instance().addToContentAndDisplay("\nExecuting " + getBlockType()  + ": " + m_blockName + "\n", _uiComponent);
		auto startTimePoint = std::chrono::high_resolution_clock::now();
		proceed = executeSpecialized();
		auto endTimePoint = std::chrono::high_resolution_clock::now();
		
		const std::string duration = TimeFormatter::formatDuration(startTimePoint, endTimePoint);
		SolverReport::instance().addToContentAndDisplay("Block execution took: " + duration + "\n", _uiComponent);
	}
	catch (const std::exception& e)
	{
		const std::string message = "Execution of block " + m_blockName + " failed: " + e.what();
		throw std::exception(message.c_str());
	}

	if (proceed)
	{
		const auto& edges = ownNode->getSucceedingNodesWithEdgeInfo();
		for (auto& edge : edges)
		{
			const std::shared_ptr<GraphNode> nextNode = edge.first;
			const EdgeInfo info = edge.second;

			const std::shared_ptr<BlockHandler> nextHandler = m_allHandler.find(nextNode)->second;
			nextHandler->setData(m_dataPerPort[info.thisNodePort], info.succeedingNodePort);
			nextHandler->executeOwnNode(nextNode);
		}
	}
}

void BlockHandler::setData(PipelineData* data, const std::string& targetPort)
{
	m_dataPerPort[targetPort] =  data;
}
