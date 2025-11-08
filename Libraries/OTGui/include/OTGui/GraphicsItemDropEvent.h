// @otlicense
// File: GraphicsItemDropEvent.h
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
#include "OTCore/Point2D.h"
#include "OTGui/GuiEvent.h"

namespace ot {
	class OT_GUI_API_EXPORT GraphicsItemDropEvent : public GuiEvent {
		OT_DECL_DEFCOPY(GraphicsItemDropEvent)
		OT_DECL_DEFMOVE(GraphicsItemDropEvent)
	public:
		GraphicsItemDropEvent() = default;
		GraphicsItemDropEvent(const ConstJsonObject& _jsonObject);
		virtual ~GraphicsItemDropEvent() = default;

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		void setEditorName(const std::string& _name) { m_editorName = _name; };
		const std::string& getEditorName() const { return m_editorName; };

		void setItemName(const std::string& _name) { m_itemName = _name; };
		const std::string& getItemName() const { return m_itemName; };

		void setScenePos(const Point2DD& _pos) { m_scenePos = _pos; };
		const Point2DD& getScenePos() const { return m_scenePos; };

	private:
		std::string m_editorName;
		std::string m_itemName;
		Point2DD m_scenePos;

	};
} // namespace ot
