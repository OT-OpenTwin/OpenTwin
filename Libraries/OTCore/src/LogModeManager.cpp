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

