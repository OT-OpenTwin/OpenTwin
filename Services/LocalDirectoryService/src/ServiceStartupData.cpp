// @otlicense

//! @file ServiceStartupData.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

// LDS header
#include "ServiceStartupData.h"

ServiceStartupData::ServiceStartupData(const SupportedService & _supportedServiceInfo) 
	: m_maxCrashRestarts(_supportedServiceInfo.getMaxCrashRestarts()), m_maxStartupRestarts(_supportedServiceInfo.getMaxStartupRestarts()),
	m_initializeAttempt(0), m_startCounter(0)
{}

// ###########################################################################################################################################################################################################################################################################################################################

// Serialization

void ServiceStartupData::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("InitializeAttempt", m_initializeAttempt, _allocator);
	_object.AddMember("StartCounter", m_startCounter, _allocator);

	_object.AddMember("MaxCrashRestarts", m_maxCrashRestarts, _allocator);
	_object.AddMember("MaxStartupRestarts", m_maxStartupRestarts, _allocator);
}
