// Frontend header
#include "ViewStateCfg.h"

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"

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

	return result;
}
