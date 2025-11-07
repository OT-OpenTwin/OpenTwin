// @otlicense

// OpenTwin header
#include "OTGui/GuiEvent.h"

ot::GuiEvent ot::GuiEvent::createForwardingEvent(const GuiEvent& _originalEvent) {
	return GuiEvent(GuiEvent::ForceHandle | GuiEvent::IgnoreNotify);
}

ot::GuiEvent::GuiEvent()
	: m_flags(EventFlag::None)
{

}

ot::GuiEvent::GuiEvent(const EventFlags& _flags)
	: m_flags(_flags)
{

}

ot::GuiEvent::GuiEvent(const ConstJsonObject& _jsonObject) : GuiEvent() {
	setFromJsonObject(_jsonObject);
}

void ot::GuiEvent::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const {
	_jsonObject.AddMember("EventFlags", m_flags.underlying(), _allocator);
}

void ot::GuiEvent::setFromJsonObject(const ConstJsonObject& _jsonObject) {
	m_flags = static_cast<EventFlags>(json::getUInt64(_jsonObject, "EventFlags"));
}
