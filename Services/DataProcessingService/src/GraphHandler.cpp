#include "GraphHandler.h"
#include "GraphNode.h"
#include "EntityBlockConnection.h"
#include "ClassFactoryBlock.h"
#include "Application.h"
#include "EntityAPI.h"


bool GraphHandler::blockDiagramIsValid(std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>& _allConnectionsByID, std::map<ot::UID, std::shared_ptr<EntityBlock>>& _allBlockEntitiesByBlockID, std::map<ot::UID, ot::UIDList>& _connectionBlockMap)
{

	m_rootNodes.clear();
	m_entityByGraphNode.clear();
	sortOutUnusedBlocks(_allBlockEntitiesByBlockID, _connectionBlockMap);
	sortOutDanglingConnections(_allBlockEntitiesByBlockID, _allConnectionsByID);
	bool graphIsValid = allRequiredConnectionsSet(_allBlockEntitiesByBlockID, _allConnectionsByID, _connectionBlockMap) &&
		allConnectionsAreValid(_allBlockEntitiesByBlockID, _allConnectionsByID) &&
		hasNoCycle(_allBlockEntitiesByBlockID, _allConnectionsByID);
	return graphIsValid;
}


bool GraphHandler::allRequiredConnectionsSet(std::map<ot::UID, std::shared_ptr<EntityBlock>>& allBlockEntitiesByBlockID, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>& _allConnectionsByID, std::map<ot::UID, ot::UIDList>& _connectionBlockMap)
{
	std::string uiErrorMessage = "";
	bool allRequiredConnectionsSet = true;

	//First we check if every block has all out and the mandatory in connections set. If not the block is not used for building a graph.
	for (auto& blockEntityByBlockID : allBlockEntitiesByBlockID)
	{
		std::shared_ptr<EntityBlock> blockEntity = blockEntityByBlockID.second;
		const bool entityHasAllIncommingConnectionsSet = entityHasIncommingConnectionsSet(_connectionBlockMap,blockEntity, _allConnectionsByID, uiErrorMessage);
		allRequiredConnectionsSet &= entityHasAllIncommingConnectionsSet;
	}
		
	if (!allRequiredConnectionsSet)
	{
		_uiComponent->displayMessage("Block diagram cannot be executed. Following errors were detected:\n" + uiErrorMessage);
	}

	return allRequiredConnectionsSet;
}

void GraphHandler::sortOutUnusedBlocks(std::map<ot::UID, std::shared_ptr<EntityBlock>>& _allBlockEntitiesByBlockID, std::map<ot::UID, ot::UIDList>& _connectionBlockMap)
{
	std::string uiInfoMessage = "";
	std::list<ot::UID> toBeErased;

	//First we check if every block has all out and the mandatory in connections set. If not the block is not used for building a graph.
	for (auto& blockEntityByBlockID : _allBlockEntitiesByBlockID)
	{
		std::shared_ptr<EntityBlock> blockEntity = blockEntityByBlockID.second;
	
		auto it = _connectionBlockMap.find(blockEntity->getEntityID());
		if (it == _connectionBlockMap.end()) {
			OT_LOG_E("BlockEntity not found - EntityID: " + std::to_string(blockEntity->getEntityID()));
			continue;
		}
		auto& connectionUIdList = it->second;

		if (connectionUIdList.size() == 0)
		{
			const std::string nameWithoutRoot = getNameWithoutRoot(blockEntity.get());
			uiInfoMessage += "Block \"" + nameWithoutRoot + "\" is not concidered furthermore, since it has no connections.\n";
			toBeErased.push_back(blockEntityByBlockID.first);
		}
	}

	//We remove all blocks without the necessary connections
	for (ot::UID& blockID : toBeErased)
	{
		_allBlockEntitiesByBlockID.erase(blockID);
	}

	if (uiInfoMessage != "")
	{
		_uiComponent->displayMessage(uiInfoMessage);
	}
}

void GraphHandler::sortOutDanglingConnections(std::map<ot::UID, std::shared_ptr<EntityBlock>>& _allBlockEntitiesByBlockID, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>& _allConnectionsByID)
{
	std::string connectionNames("");
	ot::UIDList toBeErased;
	for (auto& connectionEntByID : _allConnectionsByID)
	{
		std::shared_ptr<EntityBlockConnection> connectionEnt = connectionEntByID.second;
		auto connectionCfg = connectionEnt->getConnectionCfg();
		ot::UID destinationBlockID = connectionCfg.getDestinationUid();

		auto destinationBlockByName = _allBlockEntitiesByBlockID.find(destinationBlockID);
		if (destinationBlockByName == _allBlockEntitiesByBlockID.end())
		{
			toBeErased.push_back(connectionEntByID.first);
			connectionNames += connectionEnt->getName() + ", ";
		}
		else
		{
			auto connectorsByName =	destinationBlockByName->second->getAllConnectorsByName();
			//In case of dynamic blocks it may be that the block with the corresponding IDs still exists, but the connectors/connectables have changed.
			if (connectorsByName.find(connectionCfg.getDestConnectable()) == connectorsByName.end() && connectorsByName.find(connectionCfg.getOriginConnectable()) == connectorsByName.end())
			{
				toBeErased.push_back(connectionEntByID.first);
				connectionNames += connectionEnt->getName() + ", ";
			}
		}

		ot::UID originBlockID = connectionCfg.getOriginUid();
		auto originBlockByName = _allBlockEntitiesByBlockID.find(originBlockID);
		if (originBlockByName == _allBlockEntitiesByBlockID.end())
		{
			toBeErased.push_back(connectionEntByID.first);
			connectionNames += getNameWithoutRoot(connectionEnt.get()) + ", ";
		}
		else
		{
			auto connectorsByName = originBlockByName->second->getAllConnectorsByName();
			//In case of dynamic blocks it may be that the block with the corresponding IDs still exists, but the connectors/connectables have changed.
			if (connectorsByName.find(connectionCfg.getDestConnectable()) == connectorsByName.end() && connectorsByName.find(connectionCfg.getOriginConnectable()) == connectorsByName.end())
			{
				toBeErased.push_back(connectionEntByID.first);
				connectionNames += connectionEnt->getName() + ", ";
			}
		}
	}

	if (!toBeErased.empty())
	{
		connectionNames = connectionNames.substr(0, connectionNames.size() - 2);
		_uiComponent->displayMessage("Invalid connection(s) detected which are ignored for the execution: " + connectionNames);
		for (ot::UID eraseID: toBeErased)
		{
			_allConnectionsByID.erase(eraseID);
		}
	}
}

bool GraphHandler::allConnectionsAreValid(std::map<ot::UID, std::shared_ptr<EntityBlock>>& _allBlockEntitiesByBlockID, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>& _allConnectionsByID)
{
	bool allConnectionsAreValid = true;
	std::string connectionNames("");
	for (auto& connectionEntByID : _allConnectionsByID)
	{
		std::shared_ptr<EntityBlockConnection> connectionEnt =	connectionEntByID.second;
		auto connectionCfg = 	connectionEnt->getConnectionCfg();
		ot::UID destinationBlock = connectionCfg.getDestinationUid();
		if (_allBlockEntitiesByBlockID.find(destinationBlock) == _allBlockEntitiesByBlockID.end())
		{
			allConnectionsAreValid = false;
			connectionNames += connectionEnt->getName() + ", ";
		}
		ot::UID originBlock = connectionCfg.getOriginUid();
		if (_allBlockEntitiesByBlockID.find(originBlock) == _allBlockEntitiesByBlockID.end())
		{
			allConnectionsAreValid = false;
		}
	}

	if (!allConnectionsAreValid)
	{
		connectionNames = connectionNames.substr(0, connectionNames.size() - 2);
		_uiComponent->displayMessage("Invalid connection(s) detected: " + connectionNames);
	}

	return allConnectionsAreValid;
}

const std::string GraphHandler::getNameWithoutRoot(EntityBase* _entity)
{
	const std::string fullEntityName = _entity->getName();
	const std::string nameWithoutRoot = fullEntityName.substr(fullEntityName.find_last_of("/") + 1);
	return nameWithoutRoot;
}

bool GraphHandler::entityHasIncommingConnectionsSet(std::map<ot::UID, ot::UIDList>& _connectionBlockMap ,std::shared_ptr<EntityBlock>& blockEntity, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>& _allConnectionsByID, std::string& uiMessage)
{
	auto& allConnectorsByName = blockEntity->getAllConnectorsByName();

	auto it = _connectionBlockMap.find(blockEntity->getEntityID());
	if (it == _connectionBlockMap.end()) {
		OT_LOG_E("BlockEntity not found - EntityID: " + std::to_string(blockEntity->getEntityID()));
		uiMessage += "BlockEntity not found - EntityID: " + std::to_string(blockEntity->getEntityID());
		return false;
	}

	auto& allConnectionIDs = it->second;

	bool allIncommingConnectionsAreSet = true;
	for (auto& connectorByName : allConnectorsByName)
	{
		const ot::Connector& connector = connectorByName.second;
		assert(connector.getConnectorType() != ot::ConnectorType::UNKNOWN);
		if (connector.getConnectorType() == ot::ConnectorType::In)
		{
			bool connectionIsSet = false;
			for (auto& connectionID : allConnectionIDs)
			{
				auto connectionByID = _allConnectionsByID.find(connectionID);
				if (connectionByID!= _allConnectionsByID.end())
				{
					auto connection =  connectionByID->second;
					ot::GraphicsConnectionCfg connectionCfg = connection->getConnectionCfg();
					const std::string& originConnectableName =	connectionCfg.getOriginConnectable();
					const std::string& destConnectableName =	connectionCfg.getDestConnectable();
					if (connector.getConnectorName() == originConnectableName || connector.getConnectorName() == destConnectableName)
					{
						connectionIsSet = true;
						break;
					}
				}
			}

			if (!connectionIsSet)
			{
				const std::string nameWithoutRoot = getNameWithoutRoot(blockEntity.get());
				uiMessage += "Block \"" + nameWithoutRoot + "\" requires the port \"" + connector.getConnectorTitle() + "\" to be set.\n";
			}
			allIncommingConnectionsAreSet &= connectionIsSet;
		}
	}
	return allIncommingConnectionsAreSet;
}

bool GraphHandler::hasNoCycle(std::map<ot::UID, std::shared_ptr<EntityBlock>>& _allBlockEntitiesByBlockID, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>& _allConnectionsByID)
{
	//First we build a graph with the blocks as nodes and connections as edges
	const Graph graph = buildGraph(_allBlockEntitiesByBlockID,_allConnectionsByID);
	
	auto& allNodes = graph.getContainedNodes();
	
	for (std::shared_ptr<GraphNode> node : allNodes)
	{
		if (node->getRankIncomming() == 0)
		{
			m_rootNodes.push_back(node);
		}
	}

	bool anyCycleExists = false;
	std::string uiMessage = "";
	//Now we start with each root node and check if a cycle exists
	for (std::shared_ptr<GraphNode> node : m_rootNodes)
	{
		const bool hasCycle = graph.hasCycles(node);

		if (hasCycle)
		{
			std::shared_ptr<EntityBlock> blockEntity = m_entityByGraphNode.find(node)->second;
			const std::string nameWithoutRoot = getNameWithoutRoot(blockEntity.get());
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

Graph GraphHandler::buildGraph(std::map<ot::UID, std::shared_ptr<EntityBlock>>& _allBlockEntitiesByBlockID, std::map<ot::UID, std::shared_ptr<EntityBlockConnection>>& _allConnectionsByID)
{
	Graph graph;
	
	//First we store the blocks as nodes
	for (auto& blockEntityByBlockID : _allBlockEntitiesByBlockID)
	{
		const ot::UID& blockID = blockEntityByBlockID.first;
		const std::shared_ptr<EntityBlock> blockEntity = blockEntityByBlockID.second;

		std::shared_ptr<GraphNode> node = graph.addNode();
		m_graphNodeByBlockID[blockID] = node;
		m_entityByGraphNode[node] = blockEntity;
	}

	//Now we add the connections as edges
	for (auto& connectionByID : _allConnectionsByID)
	{
		auto connectionEnt = connectionByID.second;
		auto connectionCfg = connectionEnt->getConnectionCfg();
				
		ot::UID destinationUID = connectionCfg.getDestinationUid();
		ot::UID originUID = connectionCfg.getOriginUid();
		const std::string& destinationConnectableName = connectionCfg.getDestConnectable();
		const std::string& originConnectableName = connectionCfg.getOriginConnectable();

		//First we determine the direction which is determined by the connectable types: out -> in/optIn
		auto originBlock =_allBlockEntitiesByBlockID.find(originUID);
		auto destBlock =_allBlockEntitiesByBlockID.find(destinationUID);
		if (originBlock == _allBlockEntitiesByBlockID.end() || destBlock == _allBlockEntitiesByBlockID.end())
		{
			throw std::exception("Graph creation failed.");
		}

		const auto&	 allDestConnectorsByName = destBlock->second->getAllConnectorsByName();
		auto destConnectableByName = allDestConnectorsByName.find(destinationConnectableName);
		
		const auto&	 allOriginConnectorsByName = originBlock->second->getAllConnectorsByName();
		auto originConnectableByName = allOriginConnectorsByName.find(originConnectableName);

		if (destConnectableByName == allDestConnectorsByName.end() || originConnectableByName == allOriginConnectorsByName.end())
		{
			throw std::exception("Graph creation failed.");
		}


		assert(originConnectableByName->second.getConnectorType() != ot::ConnectorType::UNKNOWN);
		assert(destConnectableByName->second.getConnectorType() != ot::ConnectorType::UNKNOWN);

		std::shared_ptr<GraphNode> originNode = m_graphNodeByBlockID[originUID];
		std::shared_ptr<GraphNode> destinationNode = m_graphNodeByBlockID[destinationUID];

		if (originConnectableByName->second.getConnectorType() == ot::ConnectorType::Out)
		{
			originNode->addSucceedingNode(destinationNode, {originConnectableName,destinationConnectableName});
			destinationNode->addPreviousNode(originNode);
		}
		else
		{
			//One of the two connectables has to be of type out.
			if(destConnectableByName->second.getConnectorType() != ot::ConnectorType::Out)
			{
				throw std::exception("Graph creation failed.");
			}

			destinationNode->addSucceedingNode(originNode, { destinationConnectableName,originConnectableName });
			originNode->addPreviousNode(destinationNode);
		}
	}

	return graph;
}
