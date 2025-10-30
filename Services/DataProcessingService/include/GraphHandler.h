// @otlicense
// File: GraphHandler.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
	bool blockDiagramIsValid(std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>& _allConnectionsByID,std::map<ot::UID, std::shared_ptr<EntityBlock>>& _allBlockEntitiesByBlockID, std::map<ot::UID, ot::UIDList>& _connectionBlockMap);

	const std::map<ot::UID, std::shared_ptr<GraphNode>>& getgraphNodesByBlockID() const { return m_graphNodeByBlockID; };
	const std::list < std::shared_ptr<GraphNode>>& getRootNodes() const { return m_rootNodes; }

private:
	std::string m_blockFolder = ot::FolderNames::BlockFolder;
	std::map<ot::UID, std::shared_ptr<GraphNode>> m_graphNodeByBlockID;
	std::map<std::shared_ptr<GraphNode>, std::shared_ptr<EntityBlock>> m_entityByGraphNode;
	std::list < std::shared_ptr<GraphNode>> m_rootNodes;

	const std::string getNameWithoutRoot(EntityBase* _entity);

	bool allRequiredConnectionsSet(std::map<ot::UID, std::shared_ptr<EntityBlock>>& _allBlockEntitiesByBlockID, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>& _allConnectionsByID, std::map<ot::UID, ot::UIDList>& _connectionBlockMap);
	void sortOutUnusedBlocks(std::map<ot::UID, std::shared_ptr<EntityBlock>>& _allBlockEntitiesByBlockID, std::map<ot::UID, ot::UIDList>& _connectionBlockMap);
	void sortOutDanglingConnections(std::map<ot::UID, std::shared_ptr<EntityBlock>>& _allBlockEntitiesByBlockID, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>& _allConnectionsByID);

	bool allConnectionsAreValid(std::map<ot::UID, std::shared_ptr<EntityBlock>>& _allBlockEntitiesByBlockID, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>& _allConnectionsByID);
	bool entityHasIncommingConnectionsSet(std::map<ot::UID, ot::UIDList>& _connectionBlockMap,std::shared_ptr<EntityBlock>& blockEntity, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>& _allConnectionsByID, std::string& uiMessage);
	bool hasNoCycle(std::map<ot::UID, std::shared_ptr<EntityBlock>>& allBlockEntitiesByBlockID,std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>& _allConnectionsByID);
	Graph buildGraph(std::map<ot::UID, std::shared_ptr<EntityBlock>>& _allBlockEntitiesByBlockID, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>& _allConnectionsByID);
};
