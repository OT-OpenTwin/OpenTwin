// @otlicense

// OpenTwin header
#include "OTGui/Graphics/GraphicsItemMap.h"

void ot::GraphicsItemMap::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const
{}

void ot::GraphicsItemMap::setFromJsonObject(const ConstJsonObject& _jsonObject)
{}

void ot::GraphicsItemMap::addItem(UID _itemId)
{
	auto it = m_itemToConnectionsMap.find(_itemId);
	if (it == m_itemToConnectionsMap.end())
	{
		m_itemToConnectionsMap.insert(std::make_pair(_itemId, UIDList()));
	}
}

void ot::GraphicsItemMap::removeItem(UID _itemId)
{
	auto it = m_itemToConnectionsMap.find(_itemId);
	if (it != m_itemToConnectionsMap.end())
	{
		m_itemToConnectionsMap.erase(it);
	}
}

void ot::GraphicsItemMap::addConnection(UID _originItemId, UID _targetItemId, UID _connectionId)
{
	auto it = m_itemToConnectionsMap.find(_originItemId);
	if (it != m_itemToConnectionsMap.end())
	{
		// Connection added before block
		it->second.push_back(_connectionId);
	}
	else
	{
		m_itemToConnectionsMap.insert_or_assign(_originItemId, UIDList{ _connectionId });
	}
}

void ot::GraphicsItemMap::removeConnection(UID _connectionId)
{
	for (auto& pair : m_itemToConnectionsMap)
	{
		auto& connections = pair.second;
		connections.erase(std::remove(connections.begin(), connections.end(), _connectionId), connections.end());
	}
}
