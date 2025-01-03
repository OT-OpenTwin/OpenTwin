#pragma once
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "OTCore/FolderNames.h"
#include "EntityBlock.h"
#include "EntityBlockConnection.h"
#include "Graph.h"

#include<map>

class GraphHandler : public BusinessLogicHandler
{
public:
	bool blockDiagramIsValid(std::map<ot::UID, std::shared_ptr<EntityBlock>>& _allBlockEntitiesByBlockID);

	const std::map<ot::UID, std::shared_ptr<GraphNode>>& getgraphNodesByBlockID() const { return m_graphNodeByBlockID; };
	const std::list < std::shared_ptr<GraphNode>>& getRootNodes() const { return m_rootNodes; }

private:
	std::string m_blockFolder = ot::FolderNames::BlockFolder;
	std::map<ot::UID, std::shared_ptr<GraphNode>> m_graphNodeByBlockID;
	std::map<std::shared_ptr<GraphNode>, std::shared_ptr<EntityBlock>> m_entityByGraphNode;
	std::list < std::shared_ptr<GraphNode>> m_rootNodes;

	const std::string getNameWithoutRoot(EntityBase* _entity);

	bool allRequiredConnectionsSet(std::map<ot::UID, std::shared_ptr<EntityBlock>>& _allBlockEntitiesByBlockID, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>& _allConnectionsByID);
	std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> loadAllConnections(std::map<ot::UID, std::shared_ptr<EntityBlock>>& _allBlockEntitiesByBlockID);
	void sortOutUnusedBlocks(std::map<ot::UID, std::shared_ptr<EntityBlock>>& _allBlockEntitiesByBlockID);
	void sortOutDanglingConnections(std::map<ot::UID, std::shared_ptr<EntityBlock>>& _allBlockEntitiesByBlockID, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>& _allConnectionsByID);
	
	bool allConnectionsAreValid(std::map<ot::UID, std::shared_ptr<EntityBlock>>& _allBlockEntitiesByBlockID, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>& _allConnectionsByID);
	bool entityHasIncommingConnectionsSet(std::shared_ptr<EntityBlock>& blockEntity, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>& _allConnectionsByID, std::string& uiMessage);
	bool hasNoCycle(std::map<ot::UID, std::shared_ptr<EntityBlock>>& allBlockEntitiesByBlockID,std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>& _allConnectionsByID);
	Graph buildGraph(std::map<ot::UID, std::shared_ptr<EntityBlock>>& _allBlockEntitiesByBlockID, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>& _allConnectionsByID);
};
