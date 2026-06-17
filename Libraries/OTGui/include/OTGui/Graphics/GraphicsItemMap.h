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

	private:

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Helper

		std::map<std::string, ItemConnectorInfo>& getConnectorsMap(UID _itemId);
		std::list<GraphicsConnectionInfo>& getConnectionsList(UID _itemId, const std::string& _connectorName);

		//! @brief Map of item UID to a map of connectable item name to its information.
		std::map<ot::UID, std::map<std::string, ItemConnectorInfo>> m_itemToConnectorsMap;

	};

}