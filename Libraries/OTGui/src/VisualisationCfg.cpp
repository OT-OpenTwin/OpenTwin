// @otlicense
// File: VisualisationCfg.cpp
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
#include "OTGui/VisualisationCfg.h"

void ot::VisualisationCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("SetAsActiveView", m_setAsActiveView, _allocator);
	_object.AddMember("OverWriteContent", m_overrideViewerContent, _allocator);
	_object.AddMember("LoadNextChunkOnly", m_loadNextChunkOnly, _allocator);
	_object.AddMember("NextChunkStartIndex", m_nextChunkStartIndex, _allocator);
	_object.AddMember("VisualisingEntities", JsonArray(m_visualisingEntities, _allocator), _allocator);
	_object.AddMember("SupressViewHandling", m_supressViewHandling, _allocator);
	_object.AddMember("VisualisationType", JsonString(m_visualisationType, _allocator), _allocator);
	_object.AddMember("IsAppend", m_isAppend, _allocator);

	if (m_customViewFlags.has_value()) {
		_object.AddMember("CustomViewFlags", JsonArray(WidgetViewBase::toStringList(m_customViewFlags.value()), _allocator), _allocator);
	}
	else {
		_object.AddMember("CustomViewFlags", JsonNullValue(), _allocator);
	}
}

void ot::VisualisationCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_setAsActiveView = json::getBool(_object,"SetAsActiveView");
	m_overrideViewerContent = json::getBool(_object, "OverWriteContent");
	m_loadNextChunkOnly = json::getBool(_object, "LoadNextChunkOnly");
	m_nextChunkStartIndex = json::getUInt64(_object, "NextChunkStartIndex");
	m_visualisingEntities = json::getUInt64List(_object, "VisualisingEntities");
	m_supressViewHandling = json::getBool(_object, "SupressViewHandling");
	m_visualisationType = json::getString(_object, "VisualisationType");
	m_isAppend = json::getBool(_object, "IsAppend");

	if (_object["CustomViewFlags"].IsNull()) {
		m_customViewFlags = std::nullopt;
	}
	else {
		m_customViewFlags = WidgetViewBase::stringListToViewFlags(json::getStringList(_object, "CustomViewFlags"));
	}
}
