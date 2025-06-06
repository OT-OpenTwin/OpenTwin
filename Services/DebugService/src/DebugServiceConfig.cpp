//! @file DebugServiceConfig.h
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

// Service header
#include "DebugServiceConfig.h"

// OpenTwin header
#include "OTCore/Logger.h"

std::string DebugServiceConfig::toString(FeatureFlag _flag) {
	switch (_flag) {
	case FeatureFlag::FeaturesDisabled: return "FeaturesDisabled";
	case FeatureFlag::ExitOnInit: return "ExitOnInit";
	case FeatureFlag::ExitOnRun: return "ExitOnRun";
	case FeatureFlag::ExitOnPing: return "ExitOnPing";
	case FeatureFlag::ExitOnPreShutdown: return "ExitOnPreShutdown";
	case FeatureFlag::ExitOnHello: return "ExitOnHello";
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
	else if (_flag == toString(FeatureFlag::ExitOnHello)) { return FeatureFlag::ExitOnHello; }
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
	if (_features & FeatureFlag::ExitOnHello) { result.push_back(toString(FeatureFlag::ExitOnHello)); }

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
