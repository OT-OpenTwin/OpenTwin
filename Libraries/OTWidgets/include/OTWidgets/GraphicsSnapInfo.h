// @otlicense
// File: GraphicsSnapInfo.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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