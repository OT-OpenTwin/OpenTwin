#pragma
#include "OpenTwinFoundation/BusinessLogicHandler.h"
#include "OpenTwinCore/FolderNames.h"
#include "EntityBlock.h"
#include "Graph.h"

#include<map>

class ValidityHandler : public BusinessLogicHandler
{
public:
	bool blockDiagramIsValid(std::map<std::string, std::shared_ptr<EntityBlock>>& allBlockEntitiesByBlockID);

private:
	std::string _blockFolder = ot::FolderNames::BlockFolder;

	const std::string getNameWithoutRoot(std::shared_ptr<EntityBlock> blockEntity);

	bool allRequiredConnectionsSet(std::map<std::string, std::shared_ptr<EntityBlock>>& allBlockEntitiesByBlockID);
	bool entityHasIncommingConnectionsSet(std::shared_ptr<EntityBlock>& blockEntity, std::string& uiMessage);
	bool hasNoCycle(std::map<std::string, std::shared_ptr<EntityBlock>>& allBlockEntitiesByBlockID);
	Graph buildGraph(std::map<std::string, std::shared_ptr<EntityBlock>>& allBlockEntitiesByBlockID, std::map<std::shared_ptr<GraphNode>, std::shared_ptr<EntityBlock>>& entityByGraphNode);
};
