// @otlicense
// File: GraphicsItemDropEvent.cpp
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
#include "OTGui/GraphicsItemDropEvent.h"

ot::GraphicsItemDropEvent::GraphicsItemDropEvent(const ConstJsonObject& _jsonObject) 
	: GraphicsItemDropEvent()
{
	setFromJsonObject(_jsonObject);
}

void ot::GraphicsItemDropEvent::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	GuiEvent::addToJsonObject(_object, _allocator);

	_object.AddMember("EditorName", JsonString(m_editorName, _allocator), _allocator);
	_object.AddMember("ItemName", JsonString(m_itemName, _allocator), _allocator);
	_object.AddMember("ScenePos", JsonObject(m_scenePos, _allocator), _allocator);
}

void ot::GraphicsItemDropEvent::setFromJsonObject(const ConstJsonObject& _object) {
	GuiEvent::setFromJsonObject(_object);

	m_editorName = json::getString(_object, "EditorName");
	m_itemName = json::getString(_object, "ItemName");
	m_scenePos.setFromJsonObject(json::getObject(_object, "ScenePos"));
}
