#include "PipelineManager.h"
#include "OpenTwinFoundation/BusinessLogicHandler.h"

void PipelineManager::RunAll() 
{
	BusinessLogicHandler handler;
	auto blockEntities = GetAllBlockEntities();
	CreatePipelines(blockEntities);
	blockEntities.clear();
}

std::list<std::shared_ptr<EntityBlock>> PipelineManager::GetAllBlockEntities()
{
	std::string entityDict = "Processing Blocks";

	return std::list<std::shared_ptr<EntityBlock>>();
}

void PipelineManager::CreatePipelines(std::list<std::shared_ptr<EntityBlock>>& allBlockEntities)
{
}
