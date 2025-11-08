// @otlicense
// File: GraphicsConnectionDropEvent.cpp
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
#include "OTGui/GraphicsConnectionDropEvent.h"

ot::GraphicsConnectionDropEvent::GraphicsConnectionDropEvent(const ConstJsonObject& _jsonObject)
	: GraphicsConnectionDropEvent()
{
	setFromJsonObject(_jsonObject);
}

void ot::GraphicsConnectionDropEvent::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	GuiEvent::addToJsonObject(_object, _allocator);

	_object.AddMember("EditorName", JsonString(m_editorName, _allocator), _allocator);
	_object.AddMember("ConnectionCfg", JsonObject(m_connectionCfg, _allocator), _allocator);
	_object.AddMember("ScenePos", JsonObject(m_scenePos, _allocator), _allocator);
}

void ot::GraphicsConnectionDropEvent::setFromJsonObject(const ConstJsonObject& _object) {
	GuiEvent::setFromJsonObject(_object);

	m_editorName = json::getString(_object, "EditorName");
	m_connectionCfg.setFromJsonObject(json::getObject(_object, "ConnectionCfg"));
	m_scenePos.setFromJsonObject(json::getObject(_object, "ScenePos"));
}
