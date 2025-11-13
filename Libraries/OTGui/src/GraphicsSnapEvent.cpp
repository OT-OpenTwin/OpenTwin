// @otlicense
// File: GraphicsSnapEvent.cpp
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
#include "OTGui/GraphicsSnapEvent.h"

ot::GraphicsSnapEvent::GraphicsSnapEvent(const ConstJsonObject& _jsonObject) : GraphicsSnapEvent() {
	setFromJsonObject(_jsonObject);
}

void ot::GraphicsSnapEvent::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	JsonArray snapArr;
	for (const SnapInfo& snapInfo : m_snapInfos) {
		JsonObject snapObj;
		snapObj.AddMember("IsOrigin", snapInfo.isOrigin, _allocator);
		snapObj.AddMember("ConnectionCfg", JsonObject(snapInfo.connectionCfg, _allocator), _allocator);
		snapArr.PushBack(snapObj, _allocator);
	}
	_object.AddMember("SnapInfos", snapArr, _allocator);
	_object.AddMember("EditorName", JsonString(m_editorName, _allocator), _allocator);
}

void ot::GraphicsSnapEvent::setFromJsonObject(const ConstJsonObject& _object) {
	m_snapInfos.clear();

	for (const ConstJsonObject& snapObj : json::getObjectList(_object, "SnapInfos")) {
		SnapInfo snapInfo;
		snapInfo.isOrigin = json::getBool(snapObj, "IsOrigin");
		snapInfo.connectionCfg = GraphicsConnectionCfg(json::getObject(snapObj, "ConnectionCfg"));
		m_snapInfos.push_back(std::move(snapInfo));
	}

	m_editorName = json::getString(_object, "EditorName");
}

void ot::GraphicsSnapEvent::addSnapInfo(const GraphicsConnectionCfg& _connectionCfg, bool _isOrigin) {
	m_snapInfos.push_back(SnapInfo(_connectionCfg, _isOrigin));
}
