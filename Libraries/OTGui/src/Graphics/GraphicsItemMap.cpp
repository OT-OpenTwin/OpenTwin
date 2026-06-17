// @otlicense

// OpenTwin header
#include "OTGui/Graphics/GraphicsItemMap.h"

void ot::GraphicsItemMap::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const
{}

void ot::GraphicsItemMap::setFromJsonObject(const ConstJsonObject& _jsonObject)
{}

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
		getConnectionsList(_connection.getOriginUid(), _connection.getOriginConnectable()).push_back(_connection);
	}
	if (_connection.hasDestination())
	{
		getConnectionsList(_connection.getDestinationUid(), _connection.getDestinationConnectable()).push_back(_connection);
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

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

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
