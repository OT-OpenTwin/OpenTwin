// @otlicense
// File: GraphicsChangeEvent.h
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
#include "OTCore/Serializable.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTGui/GraphicsConnectionCfg.h"

// std header
#include <list>

namespace ot {

	class OT_GUI_API_EXPORT GraphicsChangeEvent : public Serializable {
		OT_DECL_NOCOPY(GraphicsChangeEvent)
	public:
		GraphicsChangeEvent() = default;
		GraphicsChangeEvent(const ConstJsonObject& _jsonObject);
		GraphicsChangeEvent(GraphicsChangeEvent&& _other) noexcept;
		virtual ~GraphicsChangeEvent();

		GraphicsChangeEvent& operator=(GraphicsChangeEvent&& _other) noexcept;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setEditorName(const std::string& _name) { m_editorName = _name; };
		const std::string& getEditorName() const { return m_editorName; };

		void addChangedItem(GraphicsItemCfg* _item) { m_changedItems.push_back(_item); };
		const std::list<GraphicsItemCfg*>& getChangedItems() const { return m_changedItems; };

		void addChangedConnection(const GraphicsConnectionCfg& _connection) { m_changedConnections.push_back(_connection); };
		const std::list<GraphicsConnectionCfg>& getChangedConnections() const { return m_changedConnections; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Helper

	private:
		void memFree();

		std::string m_editorName;
		std::list<GraphicsItemCfg*> m_changedItems;
		std::list<GraphicsConnectionCfg> m_changedConnections;
	};

}