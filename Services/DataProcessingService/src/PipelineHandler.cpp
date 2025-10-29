// @otlicense

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

#include "OTCore/LogDispatcher.h"
#include <string>
#include <ctime>
#include "SolverReport.h"
#include "OTServiceFoundation/TimeFormatter.h"

void PipelineHandler::runAll(const std::list<std::shared_ptr<GraphNode>>& _rootNodes, const std::map<ot::UID, std::shared_ptr<GraphNode>>& _graphNodesByBlockID, std::map<ot::UID, std::shared_ptr<EntityBlock>>& _allBlockEntitiesByBlockID)
{
	try
	{
		initiate(_graphNodesByBlockID, _allBlockEntitiesByBlockID);
		const std::string timeAndDate = TimeFormatter::createCurrentDateTimeString();
		SolverReport::instance().addToContent("Starting pipeline at: " + timeAndDate + "\n");

		for (std::shared_ptr<GraphNode> rootNode : _rootNodes)
		{
			std::shared_ptr<BlockHandler> handler = m_blockHandlerByGraphNode[rootNode];
			handler->executeOwnNode(rootNode);
		}
		SolverReport::instance().addToContentAndDisplay("Pipeline executed successfull.\n", _uiComponent);
	}
	catch (const std::exception& ex)
	{
		const std::string message = "Pipeline execution failed with error: " + std::string(ex.what()) + "\n";
		OT_LOG_E(message);
		SolverReport::instance().addToContent(message);
	}
}

void PipelineHandler::initiate(const std::map<ot::UID, std::shared_ptr<GraphNode>>& _graphNodesByBlockID, std::map<ot::UID, std::shared_ptr<EntityBlock>>& _allBlockEntitiesByBlockID)
{
	
	for (auto& blockEntityByBlockID : _allBlockEntitiesByBlockID)
	{
		std::shared_ptr<EntityBlock> blockEntity = blockEntityByBlockID.second;
		std::shared_ptr<GraphNode> graphNode = _graphNodesByBlockID.find(blockEntity->getEntityID())->second;
		m_blockHandlerByGraphNode[graphNode] = createBlockHandler(blockEntity);
		m_blockHandlerByGraphNode[graphNode]->setModelComponent(_modelComponent);
		m_blockHandlerByGraphNode[graphNode]->setUIComponent(_uiComponent);
	}
}

std::shared_ptr<BlockHandler> PipelineHandler::createBlockHandler(std::shared_ptr<EntityBlock> _blockEntity)
{
	EntityBlockDatabaseAccess* dbAccessEntity = dynamic_cast<EntityBlockDatabaseAccess*>(_blockEntity.get());
	if (dbAccessEntity != nullptr)
	{
		return std::make_shared<BlockHandlerDatabaseAccess>(dbAccessEntity, m_blockHandlerByGraphNode);
	}
	
	EntityBlockPython* pythonEntity = dynamic_cast<EntityBlockPython*>(_blockEntity.get());
	if (pythonEntity != nullptr)
	{
		return std::make_shared <BlockHandlerPython>(pythonEntity, m_blockHandlerByGraphNode);
	}

	EntityBlockDisplay* display = dynamic_cast<EntityBlockDisplay*>(_blockEntity.get());
	if (display != nullptr)
	{
		return std::make_shared<BlockHandlerDisplay>(display, m_blockHandlerByGraphNode);
	}
	
	EntityBlockFileWriter* fileWriter = dynamic_cast<EntityBlockFileWriter*>(_blockEntity.get());
	if (fileWriter != nullptr)
	{
		return std::make_shared<BlockHandlerFileWriter>(fileWriter, m_blockHandlerByGraphNode);
	}

	EntityBlockStorage* storage = dynamic_cast<EntityBlockStorage*>(_blockEntity.get());
	if (storage != nullptr)
	{
		return std::make_shared<BlockHandlerStorage>(storage, m_blockHandlerByGraphNode);
	}
	
	assert(0);
	throw std::exception("Not supported block type detected.");
	
}
