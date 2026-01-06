// @otlicense
// File: GuiEvent.h
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
#include "OTGui/GuiTypes.h"

namespace ot {

	class OT_GUI_API_EXPORT GuiEvent : public Serializable {
		OT_DECL_DEFCOPY(GuiEvent)
		OT_DECL_DEFMOVE(GuiEvent)
	public:
		enum EventFlag : uint64_t {
			None         = 0 << 0, //! @brief No flags are set.
			ForceHandle  = 1 << 0, //! @brief Skip DataHandle callback check and force handling the event.
			IgnoreNotify = 1 << 1  //! @brief Skip DataNotify callback after handling the event.
		};
		typedef ot::Flags<EventFlag> EventFlags;

		GuiEvent();
		GuiEvent(const EventFlags& _flags);
		GuiEvent(const ConstJsonObject& _jsonObject);
		virtual ~GuiEvent() = default;

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		//! @brief Prepares the event to be forwarded to a notifier.
		//! The base implementation removes flags that would cause the event to be forwarded again.
		virtual void setForwarding();

		void setEventFlag(EventFlag _flag, bool _set = true) { m_flags.set(_flag, _set); };
		void setEventFlags(const EventFlags& _flags) { m_flags = _flags; };
		bool isEventFlagSet(EventFlag _flag) const { return m_flags.has(_flag); };
		const EventFlags& getEventFlags() const { return m_flags; };

	private:
		EventFlags m_flags;

	};

}

OT_ADD_FLAG_FUNCTIONS(ot::GuiEvent::EventFlag, ot::GuiEvent::EventFlags)