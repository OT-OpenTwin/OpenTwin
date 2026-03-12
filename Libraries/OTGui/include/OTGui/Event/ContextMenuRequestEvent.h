// @otlicense
// File: ContextMenuRequestEvent.h
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
#include "OTGui/Event/ContextRequestData.h"
#include "OTGui/Event/GuiEvent.h"

// std header
#include <list>

namespace ot {

	class OT_GUI_API_EXPORT ContextMenuRequestEvent : public GuiEvent
	{
		OT_DECL_DEFCOPY(ContextMenuRequestEvent)
		OT_DECL_DEFMOVE(ContextMenuRequestEvent)
	public:
		explicit ContextMenuRequestEvent() = default;
		explicit ContextMenuRequestEvent(ContextRequestData* _data);
		explicit ContextMenuRequestEvent(std::unique_ptr<ContextRequestData>&& _data);
		ContextMenuRequestEvent(const ConstJsonObject& _jsonObject);
		~ContextMenuRequestEvent() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setRequestData(ContextRequestData* _data) { m_data.reset(_data); };
		void setRequestData(std::unique_ptr<ContextRequestData>&& _data) { m_data = std::move(_data); };
		ContextRequestData* getRequestData() { return m_data.get(); };
		const ContextRequestData* getRequestData() const { return m_data.get(); };
		template <typename T> T* getRequestDataAs() { return dynamic_cast<T*>(m_data.get()); };
		
	private:
		std::unique_ptr<ContextRequestData> m_data;
	};

}