// @otlicense
// File: DebugServiceConfig.cpp
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

// Service header
#include "DebugServiceConfig.h"

// OpenTwin header
#include "OTCore/LogDispatcher.h"

std::string DebugServiceConfig::toString(FeatureFlag _flag) {
	switch (_flag) {
	case FeatureFlag::FeaturesDisabled: return "FeaturesDisabled";
	case FeatureFlag::ExitOnInit: return "ExitOnInit";
	case FeatureFlag::ExitOnRun: return "ExitOnRun";
	case FeatureFlag::ExitOnPing: return "ExitOnPing";
	case FeatureFlag::ExitOnPreShutdown: return "ExitOnPreShutdown";
	default:
		OT_LOG_EAS("Unknown ConfigFlag (" + std::to_string(_flag) + ")");
		return "FeaturesDisabled";
	}
}

DebugServiceConfig::FeatureFlag DebugServiceConfig::stringToFeatureFlag(const std::string& _flag) {
	if (_flag == toString(FeatureFlag::FeaturesDisabled)) { return FeatureFlag::FeaturesDisabled; }
	else if (_flag == toString(FeatureFlag::ExitOnInit)) { return FeatureFlag::ExitOnInit; }
	else if (_flag == toString(FeatureFlag::ExitOnRun)) { return FeatureFlag::ExitOnRun; }
	else if (_flag == toString(FeatureFlag::ExitOnPing)) { return FeatureFlag::ExitOnPing; }
	else if (_flag == toString(FeatureFlag::ExitOnPreShutdown)) { return FeatureFlag::ExitOnPreShutdown; }
	else {
		OT_LOG_EAS("Unknown ConfigFlag \"" + _flag + "\"");
		return FeatureFlag::FeaturesDisabled;
	}
}

std::list<std::string> DebugServiceConfig::toStringList(const Features& _features) {
	std::list<std::string> result;

	if (_features & FeatureFlag::ExitOnInit) { result.push_back(toString(FeatureFlag::ExitOnInit)); }
	if (_features & FeatureFlag::ExitOnRun) { result.push_back(toString(FeatureFlag::ExitOnRun)); }
	if (_features & FeatureFlag::ExitOnPing) { result.push_back(toString(FeatureFlag::ExitOnPing)); }
	if (_features & FeatureFlag::ExitOnPreShutdown) { result.push_back(toString(FeatureFlag::ExitOnPreShutdown)); }

	return result;
}

DebugServiceConfig::Features DebugServiceConfig::stringListToFeatures(const std::list<std::string>& _features) {
	Features result;
	for (const std::string& flag : _features) {
		result.setFlag(stringToFeatureFlag(flag), true);
	}
	return result;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Constructor

DebugServiceConfig::DebugServiceConfig(const Features& _features, int _sleepTime)
	: m_features(_features)
{}

// ###########################################################################################################################################################################################################################################################################################################################

// Virtual functions

void DebugServiceConfig::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Features", ot::JsonArray(toStringList(m_features), _allocator), _allocator);
}

void DebugServiceConfig::setFromJsonObject(const ot::ConstJsonObject& _object) {
	if (_object.HasMember("Features")) {
		m_features = stringListToFeatures(ot::json::getStringList(_object, "Features"));
	}
}
