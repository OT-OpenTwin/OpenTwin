#pragma once
#include <list>

#include "EntityBlock.h"
#include "OpenTwinFoundation/BusinessLogicHandler.h"

class PipelineManager : public BusinessLogicHandler
{
public:
	void RunAll();
private:
	std::list<std::shared_ptr<EntityBlock>> GetAllBlockEntities();
	void CreatePipelines(std::list<std::shared_ptr<EntityBlock>>& allBlockEntities);

	bool CheckIfSourceHasOutgoingConnection(ot::Connector& connector, std::list<ot::BlockConnection>& allBlockConnections);
};
