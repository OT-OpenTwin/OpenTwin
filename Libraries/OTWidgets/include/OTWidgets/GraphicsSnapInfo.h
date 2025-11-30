// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/GraphicsChangeEvent.h"
#include "OTWidgets/WidgetTypes.h"

// std header
#include <list>

namespace ot {

	class GraphicsItem;
	class GraphicsConnectionItem;

	class OT_WIDGETS_API_EXPORT GraphicsSnapInfo {
		OT_DECL_NOCOPY(GraphicsSnapInfo)
		OT_DECL_NOMOVE(GraphicsSnapInfo)
	public:
		GraphicsSnapInfo() = default;
		~GraphicsSnapInfo() = default;

		void addOriginSnap(const GraphicsItem* _item, const GraphicsItem* _connector, const GraphicsConnectionItem* _connection);
		void addDestSnap(const GraphicsItem* _item, const GraphicsItem* _connector, const GraphicsConnectionItem* _connection);

		//! @brief Fills the provided event with the snap information stored.
		//! @param _event Event to fill.
		void fillEvent(GraphicsChangeEvent& _event) const;

	private:
		struct SnapInfo {
			OT_DECL_NOCOPY(SnapInfo)
			OT_DECL_DEFMOVE(SnapInfo)
			OT_DECL_NODEFAULT(SnapInfo)

			SnapInfo(const GraphicsItem* _item, const GraphicsItem* _connector, const GraphicsConnectionItem* _connection, double _distance)
				: item(_item), connector(_connector), connection(_connection), distance(_distance)
			{};

			const GraphicsItem* item;
			const GraphicsItem* connector;
			const GraphicsConnectionItem* connection;
			double distance;
		};
		void addSnap(const GraphicsItem* _item, const GraphicsItem* _connector, const GraphicsConnectionItem* _connection, bool _isOrigin);

		std::list<SnapInfo> m_originSnaps;
		std::list<SnapInfo> m_destSnaps;
	};

}