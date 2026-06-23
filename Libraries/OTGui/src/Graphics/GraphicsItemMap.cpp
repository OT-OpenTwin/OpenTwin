// @otlicense

// OpenTwin header
#include "OTGui/Graphics/GraphicsItemMap.h"

// std header
#include <queue>
#include <unordered_set>

ot::GraphicsItemMap::GraphicsItemMap(const ConstJsonObject& _jsonObject)
	: GraphicsItemMap()
{
	setFromJsonObject(_jsonObject);
}

void ot::GraphicsItemMap::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const
{
	JsonArray itemMapArr;
	for (const auto& itemIt : m_itemToConnectorsMap)
	{
		JsonObject itemObj;
		itemObj.AddMember("ItemID", itemIt.first, _allocator);

		JsonArray connectorsArr;
		for (const auto& connectorIt : itemIt.second)
		{
			JsonObject connectorObj;
			connectorObj.AddMember("ConnectorName", JsonString(connectorIt.first, _allocator), _allocator);

			JsonArray connectionsArr;
			for (const auto& connection : connectorIt.second.connections)
			{
				JsonObject connectionObj;
				connection.addToJsonObject(connectionObj, _allocator);
				connectionsArr.PushBack(connectionObj, _allocator);
			}
			connectorObj.AddMember("Connections", connectionsArr, _allocator);
			connectorsArr.PushBack(connectorObj, _allocator);
		}
		itemObj.AddMember("Connectors", connectorsArr, _allocator);
		itemMapArr.PushBack(itemObj, _allocator);
	}
	_jsonObject.AddMember("ItemMap", itemMapArr, _allocator);
}

void ot::GraphicsItemMap::setFromJsonObject(const ConstJsonObject& _jsonObject)
{
	this->clear();

	for (const ConstJsonObject& itemObj : json::getObjectList(_jsonObject, "ItemMap"))
	{
		UID itemId = json::getUInt64(itemObj, "ItemID");
		std::map<std::string, ItemConnectorInfo> connectorsMap;

		for (const ConstJsonObject& connectorObj : json::getObjectList(itemObj, "Connectors"))
		{
			std::string connectorName = json::getString(connectorObj, "ConnectorName");
			ItemConnectorInfo connectorInfo(connectorName);
			for (const ConstJsonObject& connectionObj : json::getObjectList(connectorObj, "Connections"))
			{
				GraphicsConnectionInfo connection(connectionObj);
				connectorInfo.connections.push_back(connection);
			}
			connectorsMap.emplace(connectorName, connectorInfo);
		}

		m_itemToConnectorsMap.emplace(itemId, connectorsMap);
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Item management

void ot::GraphicsItemMap::addItem(UID _itemId, const std::list<std::string>&_connectors)
{
	auto& connectorsMap = getConnectorsMap(_itemId);
	for (const auto& connectorName : _connectors)
	{
		auto it = connectorsMap.find(connectorName);
		if (it == connectorsMap.end())
		{
			connectorsMap.emplace(connectorName, ItemConnectorInfo(connectorName));
		}
	}
}

void ot::GraphicsItemMap::removeItem(UID _itemId)
{
	m_itemToConnectorsMap.erase(_itemId);
}

void ot::GraphicsItemMap::addConnection(const GraphicsConnectionInfo& _connection)
{
	if (_connection.hasOrigin()) 
	{
		addConnectionToList(_connection.getOriginUid(), _connection.getOriginConnectable(), _connection);
	}
	if (_connection.hasDestination())
	{
		addConnectionToList(_connection.getDestinationUid(), _connection.getDestinationConnectable(), _connection);
	}
	
}

void ot::GraphicsItemMap::removeConnection(UID _connectionId)
{
	for (auto& it : m_itemToConnectorsMap)
	{
		for (auto& connectorIt : it.second)
		{
			auto& connections = connectorIt.second.connections;
			for (auto connIt = connections.begin(); connIt != connections.end(); )
			{
				if (connIt->getUid() == _connectionId)
				{
					connIt = connections.erase(connIt);
				}
				else
				{
					++connIt;
				}
			}
		}
	}
}

void ot::GraphicsItemMap::removeConnectionFromItem(UID _itemId, UID _connectionId)
{
	auto it = m_itemToConnectorsMap.find(_itemId);
	if (it == m_itemToConnectorsMap.end())
	{
		return;
	}

	// Go trough all connectors
	for (auto& connectorIt : it->second)
	{
		auto& connections = connectorIt.second.connections;
		for (auto connIt = connections.begin(); connIt != connections.end(); )
		{
			if (connIt->getUid() == _connectionId)
			{
				connIt = connections.erase(connIt);
			}
			else
			{
				++connIt;
			}
		}
	}
}

void ot::GraphicsItemMap::removeConnectionFromItem(UID _itemId, const std::string& _connectorName, UID _connectionId)
{
	auto it = m_itemToConnectorsMap.find(_itemId);
	if (it == m_itemToConnectorsMap.end())
	{
		return;
	}

	auto connectorIt = it->second.find(_connectorName);
	if (connectorIt == it->second.end())
	{
		return;
	}

	auto& connections = connectorIt->second.connections;
	for (auto connIt = connections.begin(); connIt != connections.end(); )
	{
		if (connIt->getUid() == _connectionId)
		{
			connIt = connections.erase(connIt);
		}
		else
		{
			++connIt;
		}
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Getter

ot::UIDList ot::GraphicsItemMap::getAllIDs() const
{
	UIDList result;

	for (const auto& it : m_itemToConnectorsMap)
	{
		result.push_back(it.first);

		for (const auto& connectorIt : it.second)
		{
			for (const auto& connection : connectorIt.second.connections)
			{
				result.push_back(connection.getUid());
			}
		}
	}

	result.sort();
	result.unique();

	return result;
}

bool ot::GraphicsItemMap::hasItem(UID _itemId) const
{
	return m_itemToConnectorsMap.find(_itemId) != m_itemToConnectorsMap.end();
}

bool ot::GraphicsItemMap::hasConnector(UID _itemId, const std::string& _connectorName) const
{
	auto it = m_itemToConnectorsMap.find(_itemId);
	if (it != m_itemToConnectorsMap.end())
	{
		return it->second.find(_connectorName) != it->second.end();
	}
	return false;
}

bool ot::GraphicsItemMap::hasConnection(UID _connectionId) const
{
	for (const auto& it : m_itemToConnectorsMap)
	{
		for (const auto& connectorIt : it.second)
		{
			for (const auto& connection : connectorIt.second.connections)
			{
				if (connection.getUid() == _connectionId)
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool ot::GraphicsItemMap::itemHasConnection(UID _itemId, UID _connectionId) const
{
	auto it = m_itemToConnectorsMap.find(_itemId);
	if (it != m_itemToConnectorsMap.end())
	{
		for (const auto& connectorIt : it->second)
		{
			for (const auto& connection : connectorIt.second.connections)
			{
				if (connection.getUid() == _connectionId)
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool ot::GraphicsItemMap::itemHasConnection(UID _itemId, const std::string& _connectorName, UID _connectionId) const
{
	auto it = m_itemToConnectorsMap.find(_itemId);
	if (it != m_itemToConnectorsMap.end())
	{
		auto connectorIt = it->second.find(_connectorName);
		if (connectorIt != it->second.end())
		{
			for (const auto& connection : connectorIt->second.connections)
			{
				if (connection.getUid() == _connectionId)
				{
					return true;
				}
			}
		}
	}
	return false;
}

std::list<ot::GraphicsConnectionInfo> ot::GraphicsItemMap::getItemConnections(UID _itemId) const
{
	std::list<GraphicsConnectionInfo> ret;

	auto it = m_itemToConnectorsMap.find(_itemId);
	if (it != m_itemToConnectorsMap.end())
	{
		for (const auto& connectorIt : it->second)
		{
			std::list<ot::GraphicsConnectionInfo> connections = connectorIt.second.connections;
			ret.splice(ret.end(), std::move(connections));
		}
	}

	return ret;
}

std::list<ot::GraphicsConnectionInfo> ot::GraphicsItemMap::getItemConnections(UID _itemId, const std::string& _connectorName) const
{
	auto it = m_itemToConnectorsMap.find(_itemId);

	if (it != m_itemToConnectorsMap.end())
	{
		auto connectorIt = it->second.find(_connectorName);
		if (connectorIt != it->second.end())
		{
			return connectorIt->second.connections;
		}
	}

	return {};
}

std::map<std::string, ot::GraphicsItemMap::ItemConnectorInfo> ot::GraphicsItemMap::getItemConnectors(UID _itemId) const
{
	auto it = m_itemToConnectorsMap.find(_itemId);
	if (it != m_itemToConnectorsMap.end())
	{
		return it->second;
	}
	else
	{
		return std::map<std::string, ItemConnectorInfo>();
	}
}

ot::GraphicsItemMap::GraphicsSubTreeResult ot::GraphicsItemMap::findSubTree(UID _startItemId, const std::string& _startConnector) const
{
	GraphicsSubTreeResult result;

	// Safety: check if start item exists

	auto itemIt = m_itemToConnectorsMap.find(_startItemId);
	if (itemIt == m_itemToConnectorsMap.end())
	{
		return result;
	}

	// Visited sets (prevent infinite traversal in large graphs)

	std::unordered_set<ot::UID> visitedItems;
	std::unordered_set<ot::UID> visitedConnections;

	// Helper lambda: resolve connection endpoints safely
	// Because connection may store origin/destination in any order
	auto getOtherSide = [](const GraphicsConnectionInfo& conn, ot::UID currentItem)
		-> std::pair<ot::UID, std::string>
		{
			if (conn.getOriginUid() == currentItem)
			{
				return { conn.getDestinationUid(), conn.getDestinationConnectable() };
			}
			else
			{
				return { conn.getOriginUid(), conn.getOriginConnectable() };
			}
		};

	// Helper lambda: cycle detection against origin item rules
	auto isBackToOriginDifferentConnector =
		[_startItemId, _startConnector](ot::UID item, const std::string& connector) -> bool
		{
			return (item == _startItemId && connector != _startConnector);
		};

	// Queue for BFS traversal over items
	std::queue<ot::UID> queue;

	// mark start item as visited (but NOT added to result)
	visitedItems.insert(_startItemId);

	// Seed: process all connections from starting connector only
	auto& startConnectorMap = itemIt->second;

	auto startConnIt = startConnectorMap.find(_startConnector);
	if (startConnIt == startConnectorMap.end())
	{
		return result;
	}

	for (const GraphicsConnectionInfo& conn : startConnIt->second.connections)
	{
		if (visitedConnections.count(conn.getUid()) != 0)
		{
			continue;
		}

		visitedConnections.insert(conn.getUid());
		result.connections.push_back(conn.getUid());

		auto [otherItem, otherConnector] = getOtherSide(conn, _startItemId);

		// Cycle detection rule:
		// reaching origin item via different connector = ERROR
		if (isBackToOriginDifferentConnector(otherItem, otherConnector))
		{
			result.hasCycle = true;
			return result;
		}

		// ignore invalid endpoints
		if (otherItem == ot::invalidUID)
		{
			continue;
		}

		if (visitedItems.insert(otherItem).second)
		{
			result.items.push_back(otherItem);
			queue.push(otherItem);
		}
	}

	// BFS traversal
	while (!queue.empty())
	{
		ot::UID currentItem = queue.front();
		queue.pop();

		auto currentIt = m_itemToConnectorsMap.find(currentItem);
		if (currentIt == m_itemToConnectorsMap.end())
		{
			continue;
		}

		// Traverse ALL connectors of the current item
		for (const auto& connectorPair : currentIt->second)
		{
			const ItemConnectorInfo& connectorInfo = connectorPair.second;

			// Traverse all connections of this connector
			for (const GraphicsConnectionInfo& conn : connectorInfo.connections)
			{
				// Skip already processed connections
				if (visitedConnections.count(conn.getUid()) != 0)
				{
					continue;
				}

				visitedConnections.insert(conn.getUid());
				result.connections.push_back(conn.getUid());

				auto [otherItem, otherConnector] = getOtherSide(conn, currentItem);

				// Cycle detection rule:
				// if we reach origin item via different connector -> error
				if (isBackToOriginDifferentConnector(otherItem, otherConnector))
				{
					result.hasCycle = true;
					return result;
				}

				// ignore invalid endpoints
				if (otherItem == ot::invalidUID)
				{
					continue;
				}

				// Add new item
				if (visitedItems.insert(otherItem).second)
				{
					result.items.push_back(otherItem);
					queue.push(otherItem);
				}
			}
		}
	}

	return result;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

void ot::GraphicsItemMap::addConnectionToList(UID _itemId, const std::string& _connectorName, const GraphicsConnectionInfo& _connection)
{
	auto& connections = getConnectionsList(_itemId, _connectorName);
	for (auto& existingConnection : connections)
	{
		if (existingConnection.getUid() == _connection.getUid())
		{
			// Connection already exists, do not add it again
			return;
		}
	}
	connections.push_back(_connection);
}

std::map<std::string, ot::GraphicsItemMap::ItemConnectorInfo>& ot::GraphicsItemMap::getConnectorsMap(UID _itemId)
{
	auto it = m_itemToConnectorsMap.find(_itemId);
	if (it == m_itemToConnectorsMap.end())
	{
		it = m_itemToConnectorsMap.emplace(_itemId, std::map<std::string, ItemConnectorInfo>()).first;
	}
	return it->second;
}

std::list<ot::GraphicsConnectionInfo>& ot::GraphicsItemMap::getConnectionsList(UID _itemId, const std::string& _connectorName)
{
	auto& connectorsMap = getConnectorsMap(_itemId);
	auto it = connectorsMap.find(_connectorName);
	if (it == connectorsMap.end())
	{
		it = connectorsMap.emplace(_connectorName, ItemConnectorInfo(_connectorName)).first;
	}
	return it->second.connections;
}
