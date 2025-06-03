//! @file LogModeManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogModeManager.h"

ot::LogModeManager::LogModeManager() 
	: m_globalFlags(NO_LOG), m_globalFlagsSet(false)
{

}

ot::LogModeManager::~LogModeManager() {}

// ###########################################################################################################################################################################################################################################################################################################################

// Serialization

void ot::LogModeManager::addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) const {
	_jsonObject.AddMember("FlagsSet", m_globalFlagsSet, _allocator);

	JsonArray flagsArr;
	ot::addLogFlagsToJsonArray(m_globalFlags, flagsArr, _allocator);
	_jsonObject.AddMember("Flags", flagsArr, _allocator);
}

