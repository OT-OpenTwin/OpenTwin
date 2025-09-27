//! @file ServiceStartupData.h
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// LDS header
#include "SupportedService.h"

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/OTClassHelper.h"

//! @brief The ServiceStartupData class contains all startup counters required for monitoring a service start.
class ServiceStartupData {
	OT_DECL_DEFCOPY(ServiceStartupData)
	OT_DECL_DEFMOVE(ServiceStartupData)
	OT_DECL_NODEFAULT(ServiceStartupData)
public:
	ServiceStartupData(const SupportedService& _supportedServiceInfo);
	~ServiceStartupData() {};

	// ###########################################################################################################################################################################################################################################################################################################################

	// Serialization

	void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const;
	
	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter/Getter

	void setMaxCrashRestarts(unsigned int _restarts) { m_maxCrashRestarts = _restarts; };
	unsigned int getMaxCrashRestarts() const { return m_maxCrashRestarts; };

	void setMaxStartupRestarts(unsigned int _restarts) { m_maxStartupRestarts = _restarts; };
	unsigned int getMaxStartupRestarts() const { return m_maxStartupRestarts; };

	void resetIniAttempt() { m_initializeAttempt = 0; };
	unsigned int incrIniAttempt() { return ++m_initializeAttempt; };
	unsigned int getIniAttempt() const { return m_initializeAttempt; };

	void resetStartCounter() { m_startCounter = 0; };
	unsigned int incrStartCounter() { return ++m_startCounter; };
	unsigned int getStartCounter() const { return m_startCounter; };

private:
	unsigned int        m_startCounter;
	unsigned int        m_initializeAttempt;

	unsigned int        m_maxCrashRestarts;
	unsigned int        m_maxStartupRestarts;
};