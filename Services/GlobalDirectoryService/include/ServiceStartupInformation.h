#pragma once

// Project header
#include "ServiceInformation.h"
#include "SessionInformation.h"

// C++ header
#include <string>

class ServiceStartupInformation {
public:
	ServiceStartupInformation();
	ServiceStartupInformation(const std::string& _serviceName, const std::string& _serviceType, const std::string& _sessionServiceURL, const std::string& _sessionID);
	ServiceStartupInformation(const ServiceInformation& _service, const SessionInformation& _session);
	ServiceStartupInformation(const ServiceStartupInformation& _other);
	virtual ~ServiceStartupInformation();

	ServiceStartupInformation& operator = (const ServiceStartupInformation& _other);
	bool operator == (const ServiceStartupInformation& _other);
	bool operator != (const ServiceStartupInformation& _other);

	void setServiceInformation(const ServiceInformation& _info) { m_serviceInfo = _info; }
	const ServiceInformation& serviceInformation(void) const { return m_serviceInfo; }

	void setSessionInformation(const SessionInformation& _info) { m_sessionInfo = _info; }
	const SessionInformation& sessionInformation(void) const { return m_sessionInfo; }

private:
	ServiceInformation		m_serviceInfo;
	SessionInformation		m_sessionInfo;
};