#include "ValidityHandler.h"

#include "GraphNode.h"

bool ValidityHandler::blockDiagramIsValid(std::map<std::string, std::shared_ptr<EntityBlock>>& allBlockEntitiesByBlockID)
{
	return allRequiredConnectionsSet(allBlockEntitiesByBlockID) && hasNoCycle(allBlockEntitiesByBlockID);
}


bool ValidityHandler::allRequiredConnectionsSet(std::map<std::string, std::shared_ptr<EntityBlock>>& allBlockEntitiesByBlockID)
{
	std::string uiErrorMessage = "";
	std::string uiInfoMessage = "";
	bool allRequiredConnectionsSet = true;
	std::list<std::string> toBeErased;
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
	
	for (std::string& blockID : toBeErased)
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


const std::string ValidityHandler::getNameWithoutRoot(std::shared_ptr<EntityBlock> blockEntity)
{
	const std::string fullEntityName = blockEntity->getName();
	const std::string nameWithoutRoot = fullEntityName.substr(fullEntityName.find_first_of("/") + 1, fullEntityName.size());
	return nameWithoutRoot;
}

bool ValidityHandler::entityHasIncommingConnectionsSet(std::shared_ptr<EntityBlock>& blockEntity, std::string& uiMessage)
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
				if (connection.destConnectable() == connector.getConnectorName() || connection.originConnectable() == connector.getConnectorName())
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

bool ValidityHandler::hasNoCycle(std::map<std::string, std::shared_ptr<EntityBlock>>& allBlockEntitiesByBlockID)
{
	std::map<std::shared_ptr<GraphNode>, std::shared_ptr<EntityBlock>> entityByGraphNode;
	const Graph graph = buildGraph(allBlockEntitiesByBlockID,entityByGraphNode);
	auto& allNodes = graph.getContainedNodes();
	std::list<std::shared_ptr<GraphNode>> rootNodes;

	for (std::shared_ptr<GraphNode> node : allNodes)
	{
		if (node->getRankIncomming() == 0)
		{
			rootNodes.push_back(node);
		}
	}

	bool anyCycleExists = false;
	std::string uiMessage = "";
	for (std::shared_ptr<GraphNode> node : rootNodes)
	{
		const bool hasCycle = graph.hasCycles(node);

		if (hasCycle)
		{
			std::shared_ptr<EntityBlock> blockEntity = entityByGraphNode.find(node)->second;
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

Graph ValidityHandler::buildGraph(std::map<std::string, std::shared_ptr<EntityBlock>>& allBlockEntitiesByBlockID, std::map<std::shared_ptr<GraphNode>, std::shared_ptr<EntityBlock>>& entityByGraphNode)
{
	Graph graph;
	std::map<std::string, std::shared_ptr<GraphNode>> graphNodeByBlockID;
	

	for (auto& blockEntityByBlockID : allBlockEntitiesByBlockID)
	{
		const std::string& blockID = blockEntityByBlockID.first;
		std::shared_ptr<GraphNode> node = graph.addNode();
		graphNodeByBlockID[blockID] = node;
	}

	for (auto& blockEntityByBlockID : allBlockEntitiesByBlockID)
	{
		std::shared_ptr<EntityBlock> blockEntity = blockEntityByBlockID.second;
		const std::string& blockID = blockEntityByBlockID.first;

		auto& connections = blockEntity->getAllConnections();
		auto& connectorsByName = blockEntity->getAllConnectorsByName();
		for (const ot::GraphicsConnectionCfg& connection : connections)
		{
			const std::string* connectorName = nullptr;
			const std::string* pairedBlockID = nullptr;

			if (connection.destUid() == blockID)
			{
				connectorName = &connection.destConnectable();
				pairedBlockID = &connection.originUid();
			}
			else
			{
				connectorName = &connection.originConnectable();
				pairedBlockID = &connection.destUid();
			}

			auto connectorByName = connectorsByName.find(*connectorName);
			ot::Connector connector = connectorByName->second;
			assert(connector.getConnectorType() != ot::ConnectorType::UNKNOWN);

			std::shared_ptr<GraphNode> thisNode = graphNodeByBlockID[blockID];
			std::shared_ptr<GraphNode> pairedNode = graphNodeByBlockID[*pairedBlockID];

			if (connector.getConnectorType() == ot::ConnectorType::Out)
			{
				thisNode->addSucceedingNode(pairedNode);
			}
			else
			{
				thisNode->addPreviousNode(pairedNode);
			}
		}
	}
	return graph;
}
