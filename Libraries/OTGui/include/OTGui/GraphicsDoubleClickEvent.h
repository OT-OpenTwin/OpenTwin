// @otlicense
// File: GraphicsDoubleClickEvent.h
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
#include "OTGui/GuiEvent.h"

namespace ot {

	class OT_GUI_API_EXPORT GraphicsDoubleClickEvent : public GuiEvent {
		OT_DECL_DEFCOPY(GraphicsDoubleClickEvent)
		OT_DECL_DEFMOVE(GraphicsDoubleClickEvent)
	public:
		GraphicsDoubleClickEvent();
		GraphicsDoubleClickEvent(const ConstJsonObject& _jsonObject);
		virtual ~GraphicsDoubleClickEvent() = default;

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		void setEditorName(const std::string& _name) { m_editorName = _name; };
		void setEditorName(std::string&& _name) { m_editorName = std::move(_name); };
		const std::string& getEditorName() const { return m_editorName; };

		void setItemName(const std::string& _name) { m_itemName = _name; };
		void setItemName(std::string&& _name) { m_itemName = std::move(_name); };
		const std::string& getItemName() const { return m_itemName; };

		void setItemUid(const UID& _uid) { m_itemUid = _uid; };
		const UID& getItemUid() const { return m_itemUid; };

	private:
		std::string m_editorName;
		std::string m_itemName;
		UID m_itemUid;

	};

}
