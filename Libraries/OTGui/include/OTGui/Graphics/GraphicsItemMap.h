// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTGui/GuiTypes.h"
#include "OTGui/Graphics/GraphicsConnectionInfo.h"

// std header
#include <map>
#include <list>

namespace ot
{

	class OT_GUI_API_EXPORT GraphicsItemMap : public Serializable
	{
		OT_DECL_DEFCOPY(GraphicsItemMap)
		OT_DECL_DEFMOVE(GraphicsItemMap)
	public:
		struct ItemConnectorInfo
		{
			ItemConnectorInfo() = default;
			ItemConnectorInfo(const std::string& _name) : name(_name) {};
			~ItemConnectorInfo() = default;

			std::string name; //! @brief Name of the connectable item.
			std::list<GraphicsConnectionInfo> connections; //! @brief List of connections associated with this connectable item.
		};

		struct GraphicsSubTreeResult
		{
			std::vector<UID> items;
			std::vector<UID> connections;
			bool hasCycle = false;
		};

		explicit GraphicsItemMap() = default;
		explicit GraphicsItemMap(const ConstJsonObject& _jsonObject);
		virtual ~GraphicsItemMap() = default;

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Item management

		void clear() { m_itemToConnectorsMap.clear(); };

		void addItem(UID _itemId, const std::list<std::string>& _connectors);
		void removeItem(UID _itemId);

		void addConnection(const GraphicsConnectionInfo& _connection);
		void removeConnection(UID _connectionId);
		void removeConnectionFromItem(UID _itemId, UID _connectionId);
		void removeConnectionFromItem(UID _itemId, const std::string& _connectorName, UID _connectionId);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Getter

		bool hasItem(UID _itemId) const;
		bool hasConnector(UID _itemId, const std::string& _connectorName) const;
		bool hasConnection(UID _connectionId) const;
		bool itemHasConnection(UID _itemId, UID _connectionId) const;
		bool itemHasConnection(UID _itemId, const std::string& _connectorName, UID _connectionId) const;

		std::list<GraphicsConnectionInfo> getItemConnections(UID _itemId) const;
		std::list<GraphicsConnectionInfo> getItemConnections(UID _itemId, const std::string& _connectorName) const;

		//! @brief Finds a connected subtree in the graphics scene starting from a given item connector.
		//! This function performs a breadth-first traversal over the scene graph starting at the
		//! specified item and connector. The graph is treated as undirected at the connector level,
		//! while respecting the directionality stored inside each GraphicsConnectionInfo.
		//!
		//! The traversal explores all items and connections reachable from the starting connector,
		//! except for the starting item itself, which is never included in the result set.
		//!
		//! Each item is expanded by traversing all of its connectors and all connections attached
		//! to those connectors. Each connection is processed at most once.
		//!
		//! A cycle condition is detected if the traversal reaches the starting item again through
		//! any connector other than the original starting connector. In this case, the traversal
		//! is aborted and the result will indicate failure.
		//!
		//! Cycles that remain fully inside the discovered subtree and do not involve returning to
		//! the starting item are allowed and do not affect traversal.
		//!
		//! @note The traversal treats connections as undirected edges, but resolves direction
		//!       using origin/destination information inside GraphicsConnectionInfo.
		//!
		//! @note Complexity is O(N + M), where N is number of visited items and M is number
		//!       of visited connections.
		//!
		//! @param _startItemId UID of the starting item.
		//! @param _startConnector Name of the connector on the starting item used as entry point.
		//!
		//! @return GraphicsSubTreeResult containing:
		//!         - List of reached item UIDs (excluding the start item)
		//!         - List of traversed connection UIDs
		//!         - Cycle flag indicating invalid return to start item via different connector
		GraphicsSubTreeResult findSubTree(ot::UID _startItemId, const std::string& _startConnector) const;

	private:

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Helper

		std::map<std::string, ItemConnectorInfo>& getConnectorsMap(UID _itemId);
		std::list<GraphicsConnectionInfo>& getConnectionsList(UID _itemId, const std::string& _connectorName);

		//! @brief Map of item UID to a map of connectable item name to its information.
		std::map<ot::UID, std::map<std::string, ItemConnectorInfo>> m_itemToConnectorsMap;

	};

}