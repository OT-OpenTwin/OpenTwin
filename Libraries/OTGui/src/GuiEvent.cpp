// @otlicense
// File: GuiEvent.cpp
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

// OpenTwin header
#include "OTGui/GuiEvent.h"

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

void ot::GuiEvent::setForwarding() {
	m_flags.set(EventFlag::ForceHandle);
	m_flags.set(EventFlag::IgnoreNotify);
}
