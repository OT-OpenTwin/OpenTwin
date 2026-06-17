// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTGui/GuiTypes.h"

// std header
#include <map>

namespace ot
{

	class OT_GUI_API_EXPORT GraphicsItemMap : public Serializable
	{
		OT_DECL_DEFCOPY(GraphicsItemMap)
		OT_DECL_DEFMOVE(GraphicsItemMap)
	public:
		struct ItemConnector
		{
			std::string name;
			UIDList connectionIds;
		};

		struct ConnectionInfo
		{
			UID originId;
			std::string originConnector;
			UID targetId;
			std::string targetConnector;
		};

		GraphicsItemMap() = default;
		virtual ~GraphicsItemMap() = default;

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		void addItem(UID _itemId);
		void removeItem(UID _itemId);

		void addConnection(UID _originItemId, UID _targetItemId, UID _connectionId);
		void removeConnection(UID _connectionId);

	private:
		std::map<ot::UID, ot::UIDList> m_itemToConnectionsMap;
		std::map<ot::UID, std::pair<UID, UID>> m_connectionToItemsMap;

	};

}