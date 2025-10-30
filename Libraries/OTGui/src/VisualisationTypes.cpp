// @otlicense
// File: VisualisationTypes.cpp
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
#include "OTGui/VisualisationTypes.h"

std::optional<ot::WidgetViewBase::ViewFlags> ot::VisualisationTypes::getCustomViewFlags(VisualisationType _visType) const {
	auto it = m_customViewFlags.find(_visType);
	if (it != m_customViewFlags.end()) {
		return it->second;
	}
	else {
		return std::nullopt;
	}
}

void ot::VisualisationTypes::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("VisualisationTypes", static_cast<uint64_t>(m_visualisations.data()), _allocator);

	JsonArray customFlagsArr;
	for (const auto& it : m_customViewFlags) {
		JsonObject flagObj;
		flagObj.AddMember("Vis", static_cast<uint64_t>(it.first), _allocator);
		flagObj.AddMember("Flags", JsonArray(WidgetViewBase::toStringList(it.second), _allocator), _allocator);
		customFlagsArr.PushBack(flagObj, _allocator);
	}
	_object.AddMember("CustomViewFlags", customFlagsArr, _allocator);
}

void ot::VisualisationTypes::setFromJsonObject(const ot::ConstJsonObject& _object) {
	if (_object.HasMember("VisualisationTypes")) {
		m_visualisations = static_cast<VisualisationType>(json::getUInt64(_object, "VisualisationTypes"));
	}
	else {
		m_visualisations = None;
	}

	m_customViewFlags.clear();
	if (_object.HasMember("CustomViewFlags")) {
		for (const auto& flagObj : json::getObjectList(_object, "CustomViewFlags")) {
			auto visType = static_cast<VisualisationType>(json::getUInt64(flagObj, "Vis"));
			auto flags = WidgetViewBase::stringListToViewFlags(json::getStringList(flagObj, "Flags"));
			m_customViewFlags.insert_or_assign(visType, flags);
		}
	}
}
