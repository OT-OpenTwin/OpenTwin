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

ot::LogModeManager::LogModeManager(const LogModeManager& _other) 
	: m_globalFlags(_other.m_globalFlags), m_globalFlagsSet(_other.m_globalFlagsSet)
{

}

ot::LogModeManager::~LogModeManager() {}

ot::LogModeManager& ot::LogModeManager::operator=(const LogModeManager& _other) {
	if (this == &_other) return *this;

	m_globalFlags = _other.m_globalFlags;
	m_globalFlagsSet = _other.m_globalFlagsSet;

	return *this;
}
