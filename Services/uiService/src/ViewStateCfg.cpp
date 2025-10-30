// @otlicense
// File: ViewStateCfg.cpp
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

// Frontend header
#include "ViewStateCfg.h"

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/LogDispatcher.h"

namespace intern {
	static const std::string c_viewConfigVersion{ "1" };
}

ViewStateCfg::ViewStateCfg() {}

ViewStateCfg::ViewStateCfg(ViewStateCfg&& _other) noexcept :
	m_viewConfig(std::move(_other.m_viewConfig))
{}

ViewStateCfg::~ViewStateCfg() {
}

std::string ViewStateCfg::toJson(void) const {
	ot::JsonDocument doc;
	doc.AddMember("Version", ot::JsonString(intern::c_viewConfigVersion, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember("ViewCfg", ot::JsonString(m_viewConfig, doc.GetAllocator()), doc.GetAllocator());
	return doc.toJson();
}

ViewStateCfg ViewStateCfg::fromJson(const std::string& _json) {
	ViewStateCfg result;

	if (!_json.empty()) {
		ot::JsonDocument doc;

		if (!doc.fromJson(_json)) {
			OT_LOG_W("Invalid view state format. Fallback to default state.");
		}
		else if (!doc.IsObject()) {
			OT_LOG_W("Invalid view state syntax. Fallback to default state.");
		}
		else if (!doc.HasMember("Version") || !doc.HasMember("ViewCfg")) {
			OT_LOG_W("Invalid view state structure. Fallback to default state.");
		}
		else if (!doc["Version"].IsString() || !doc["ViewCfg"].IsString()) {
			OT_LOG_W("Invalid view state members. Fallback to default state.");
		}
		else if (doc["Version"].GetString() != intern::c_viewConfigVersion) {
			OT_LOG_W("Invalid view state version. Fallback to default state.");
		}
		else {
			result.m_viewConfig = doc["ViewCfg"].GetString();
		}
	}

	return result;
}
