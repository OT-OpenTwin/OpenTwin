// @otlicense

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

		static GuiEvent createForwardingEvent(const GuiEvent& _originalEvent);

		GuiEvent();
		GuiEvent(const EventFlags& _flags);
		GuiEvent(const ConstJsonObject& _jsonObject);
		virtual ~GuiEvent() = default;

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		void setEventFlag(EventFlag _flag, bool _set = true) { m_flags.set(_flag, _set); };
		void setEventFlags(const EventFlags& _flags) { m_flags = _flags; };
		bool isEventFlagSet(EventFlag _flag) const { return m_flags.has(_flag); };
		const EventFlags& getEventFlags() const { return m_flags; };

	private:
		EventFlags m_flags;

	};

}

OT_ADD_FLAG_FUNCTIONS(ot::GuiEvent::EventFlag, ot::GuiEvent::EventFlags)