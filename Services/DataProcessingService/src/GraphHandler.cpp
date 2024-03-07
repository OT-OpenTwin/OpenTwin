#include "GraphHandler.h"

#include "GraphNode.h"

bool GraphHandler::blockDiagramIsValid(std::map<ot::UID, std::shared_ptr<EntityBlock>>& allBlockEntitiesByBlockID)
{
	_rootNodes.clear();
	_entityByGraphNode.clear();
	return allRequiredConnectionsSet(allBlockEntitiesByBlockID) && hasNoCycle(allBlockEntitiesByBlockID);
}


bool GraphHandler::allRequiredConnectionsSet(std::map<ot::UID, std::shared_ptr<EntityBlock>>& allBlockEntitiesByBlockID)
{
	std::string uiErrorMessage = "";
	std::string uiInfoMessage = "";
	bool allRequiredConnectionsSet = true;
	std::list<ot::UID> toBeErased;
	for (auto& blockEntityByBlockID : allBlockEntitiesByBlockID)
	{
		std::shared_ptr<EntityBlock> blockEntity = blockEntityByBlockID.second;
		auto& allConnections = blockEntity->getAllConnections();
		if (allConnections.size()  == 0)
		{
			const std::string nameWithoutRoot = getNameWithoutRoot(blockEntity);
			uiInfoMessage += "Block \"" + nameWithoutRoot + "\" is not concidered furthermore, since it has no connections.\n";
			toBeErased.push_back(blockEntityByBlockID.first);
		}
		else
		{
			const bool entityHasAllIncommingConnectionsSet = entityHasIncommingConnectionsSet(blockEntity, uiErrorMessage);
			allRequiredConnectionsSet &= entityHasAllIncommingConnectionsSet;
		}
	}
	
	for (ot::UID& blockID : toBeErased)
	{
		allBlockEntitiesByBlockID.erase(blockID);
	}

	if (uiInfoMessage != "")
	{
		_uiComponent->displayMessage(uiInfoMessage);
	}
	
	if (!allRequiredConnectionsSet)
	{
		_uiComponent->displayMessage("Block diagram cannot be executed. Following errors were detected:\n" + uiErrorMessage);
	}

	return allRequiredConnectionsSet;
}


const std::string GraphHandler::getNameWithoutRoot(std::shared_ptr<EntityBlock> blockEntity)
{
	const std::string fullEntityName = blockEntity->getName();
	const std::string nameWithoutRoot = fullEntityName.substr(fullEntityName.find_first_of("/") + 1, fullEntityName.size());
	return nameWithoutRoot;
}

bool GraphHandler::entityHasIncommingConnectionsSet(std::shared_ptr<EntityBlock>& blockEntity, std::string& uiMessage)
{
	auto& allConnectorsByName = blockEntity->getAllConnectorsByName();
	auto& allConnections = blockEntity->getAllConnections();
	bool allIncommingConnectionsAreSet = true;
	for (auto& connectorByName : allConnectorsByName)
	{
		const ot::Connector& connector = connectorByName.second;
		assert(connector.getConnectorType() != ot::ConnectorType::UNKNOWN);
		if (connector.getConnectorType() == ot::ConnectorType::In)
		{
			bool connectionIsSet = false;
			for (auto& connection : allConnections)
			{
			//	if (connection.destConnectable() == connector.getConnectorName() || connection.originConnectable() == connector.getConnectorName())
				{
					connectionIsSet = true;
					break;
				}
			}
			if (!connectionIsSet)
			{
				const std::string nameWithoutRoot = getNameWithoutRoot(blockEntity);
				uiMessage += "Block \"" + nameWithoutRoot + "\" requires the port \"" + connector.getConnectorTitle() + "\" to be set.\n";
			}
			allIncommingConnectionsAreSet &= connectionIsSet;
		}
	}
	return allIncommingConnectionsAreSet;
}

bool GraphHandler::hasNoCycle(std::map<ot::UID, std::shared_ptr<EntityBlock>>& allBlockEntitiesByBlockID)
{
	const Graph graph = buildGraph(allBlockEntitiesByBlockID);
	auto& allNodes = graph.getContainedNodes();
	
	for (std::shared_ptr<GraphNode> node : allNodes)
	{
		if (node->getRankIncomming() == 0)
		{
			_rootNodes.push_back(node);
		}
	}

	bool anyCycleExists = false;
	std::string uiMessage = "";
	for (std::shared_ptr<GraphNode> node : _rootNodes)
	{
		const bool hasCycle = graph.hasCycles(node);

		if (hasCycle)
		{
			std::shared_ptr<EntityBlock> blockEntity = _entityByGraphNode.find(node)->second;
			const std::string nameWithoutRoot = getNameWithoutRoot(blockEntity);
			uiMessage += "Cycle detected starting from block \"" + nameWithoutRoot + "\"\n";
		}
		anyCycleExists &= hasCycle;
	}
	if (anyCycleExists)
	{
		_uiComponent->displayMessage(uiMessage);
	}
	return !anyCycleExists;
}

Graph GraphHandler::buildGraph(std::map<ot::UID, std::shared_ptr<EntityBlock>>& allBlockEntitiesByBlockID)
{
	Graph graph;
	

	for (auto& blockEntityByBlockID : allBlockEntitiesByBlockID)
	{
		const ot::UID& blockID = blockEntityByBlockID.first;
		const std::shared_ptr<EntityBlock> blockEntity = blockEntityByBlockID.second;

		std::shared_ptr<GraphNode> node = graph.addNode();
		_graphNodeByBlockID[blockID] = node;
		_entityByGraphNode[node] = blockEntity;
	}

	for (auto& blockEntityByBlockID : allBlockEntitiesByBlockID)
	{
		std::shared_ptr<EntityBlock> blockEntity = blockEntityByBlockID.second;
		const ot::UID& blockID = blockEntityByBlockID.first;

		auto& connections = blockEntity->getAllConnections();
		auto& connectorsByName = blockEntity->getAllConnectorsByName();
	//	for (const ot::GraphicsConnectionCfg& connection : connections)
	//	{
	//		const std::string* thisConnectorName = nullptr;
	//		const std::string* otherConnectorName = nullptr;
	//		const std::string* pairedBlockID = nullptr;

	//		if (connection.destUid() == blockID)
	//		{
	//			thisConnectorName = &connection.destConnectable();
	//			otherConnectorName = &connection.originConnectable();
	//			pairedBlockID = &connection.originUid();
	//		}
	//		else
	//		{
	//			thisConnectorName = &connection.originConnectable();
	//			otherConnectorName = &connection.destConnectable();
	//			pairedBlockID = &connection.destUid();
	//		}

	//		//Some blocks have dynamic connectors. We need to check if the connection entry is still valid.
	//		auto connectorByName = connectorsByName.find(*thisConnectorName);
	//		auto connectedBlock = allBlockEntitiesByBlockID.find(*pairedBlockID);
	//		auto connectorsOfConnectedBlock = connectedBlock->second->getAllConnectorsByName();
	//		if (connectorByName != connectorsByName.end() && connectorsOfConnectedBlock.find(*otherConnectorName) != connectorsOfConnectedBlock.end())
	//		{
	//			ot::Connector connector = connectorByName->second;
	//			assert(connector.getConnectorType() != ot::ConnectorType::UNKNOWN);

	//			std::shared_ptr<GraphNode> thisNode = _graphNodeByBlockID[blockID];
	//			std::shared_ptr<GraphNode> pairedNode = _graphNodeByBlockID[*pairedBlockID];

	//			if (connector.getConnectorType() == ot::ConnectorType::Out)
	//			{
	//				thisNode->addSucceedingNode(pairedNode, {*thisConnectorName,*otherConnectorName});
	//			}
	//			else
	//			{
	//				thisNode->addPreviousNode(pairedNode);
	//			}
	//		}
	//	}
	}
	return graph;
}
