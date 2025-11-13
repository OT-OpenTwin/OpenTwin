// @otlicense
// File: GraphicsSnapEvent.h
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
#include "OTGui/GraphicsConnectionCfg.h"

// std header
#include <list>

namespace ot {

	class OT_GUI_API_EXPORT GraphicsSnapEvent : public GuiEvent {
		OT_DECL_DEFCOPY(GraphicsSnapEvent)
		OT_DECL_DEFMOVE(GraphicsSnapEvent)
	public:
		struct SnapInfo {
			OT_DECL_DEFCOPY(SnapInfo)
			OT_DECL_DEFMOVE(SnapInfo)
			
			SnapInfo() = default;
			SnapInfo(const GraphicsConnectionCfg& _connectionCfg, bool _isOrigin)
				: connectionCfg(_connectionCfg), isOrigin(_isOrigin) {
			}
			SnapInfo(GraphicsConnectionCfg&& _connectionCfg, bool _isOrigin)
				: connectionCfg(std::move(_connectionCfg)), isOrigin(_isOrigin) {
			}

			GraphicsConnectionCfg connectionCfg;
			bool isOrigin = false; // true: origin, false: destination
		};

		GraphicsSnapEvent() = default;
		GraphicsSnapEvent(const ConstJsonObject& _jsonObject);
		virtual ~GraphicsSnapEvent() = default;

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		bool isEmpty() const { return m_snapInfos.empty(); };

		void setEditorName(const std::string& _name) { m_editorName = _name; };
		const std::string& getEditorName() const { return m_editorName; };

		void addSnapInfo(const GraphicsConnectionCfg& _connectionCfg, bool _isOrigin);
		const std::list<SnapInfo>& getSnapInfos() const { return m_snapInfos; };

	private:
		std::string m_editorName;
		std::list<SnapInfo> m_snapInfos;

	};

}