//! @file ServiceInformation.h
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// LDS header
#include "SessionInformation.h"

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTCore/OTClassHelper.h"

// std header
#include <string>

//! @brief The ServiceInformation class contains all basic information about a service that is mananged by the LDS.
class ServiceInformation {
public:
	ServiceInformation();
	ServiceInformation(const std::string& _name, const std::string& _type);
	ServiceInformation(const std::string& _name, const std::string& _type, const SessionInformation& _sessionInfo);
	ServiceInformation(const ServiceInformation& _other);
	ServiceInformation(ServiceInformation&& _other) noexcept;
	virtual ~ServiceInformation();

	ServiceInformation& operator = (const ServiceInformation& _other);
	ServiceInformation& operator = (ServiceInformation&& _other) noexcept;

	bool operator == (const ServiceInformation& _other) const;
	bool operator != (const ServiceInformation& _other) const;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Serialization

	void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const;
	
	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter/Getter

	void setName(const std::string& _name) { m_name = _name; };
	const std::string& getName(void) const { return m_name; };

	void setType(const std::string& _type) { m_type = _type; };
	const std::string getType(void) const { return m_type; };

	void setSessionInformation(const SessionInformation& _sessionInformation) { m_session = _sessionInformation; };
	const SessionInformation& getSessionInformation(void) const { return m_session; };

	void setSessionId(const std::string& _sessionId) { m_session.setID(_sessionId); };
	const std::string& getSessionId(void) const { return m_session.getId(); };

	void setSessionServiceURL(const std::string& _url) { m_session.setSessionServiceURL(_url); };
	const std::string& getSessionServiceURL(void) const { return m_session.getSessionServiceURL(); };

	void setMaxCrashRestarts(unsigned int _restarts) { m_maxCrashRestarts = _restarts; };
	unsigned int getMaxCrashRestarts(void) const { return m_maxCrashRestarts; };

	void setMaxStartupRestarts(unsigned int _restarts) { m_maxStartupRestarts = _restarts; };
	unsigned int getMaxStartupRestarts(void) const { return m_maxStartupRestarts; };

	void resetIniAttempt(void) { m_initializeAttempt = 0; };
	void incrIniAttempt(void) { m_initializeAttempt++; };
	unsigned int getIniAttempt(void) const { return m_initializeAttempt; };

	void resetStartCounter(void) { m_startCounter = 0; };
	void incrStartCounter(void) { m_startCounter++; };
	unsigned int getStartCounter(void) const { return m_startCounter; };

private:
	std::string        m_name;
	std::string        m_type;
	SessionInformation m_session;

	unsigned int       m_startCounter;
	unsigned int       m_initializeAttempt;

	unsigned int       m_maxCrashRestarts;
	unsigned int       m_maxStartupRestarts;
};