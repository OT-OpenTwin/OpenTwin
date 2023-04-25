#include "ServiceStartupInformation.h"

ServiceStartupInformation::ServiceStartupInformation() {}

ServiceStartupInformation::ServiceStartupInformation(const std::string& _serviceName, const std::string& _serviceType, const std::string& _sessionServiceURL, const std::string& _sessionID)
	: m_serviceInfo(_serviceName, _serviceType), m_sessionInfo(_sessionID, _sessionServiceURL) {}

ServiceStartupInformation::ServiceStartupInformation(const ServiceInformation& _service, const SessionInformation& _session)
	: m_serviceInfo(_service), m_sessionInfo(_session)
{}

ServiceStartupInformation::ServiceStartupInformation(const ServiceStartupInformation& _other)
	: m_serviceInfo(_other.m_serviceInfo), m_sessionInfo(_other.m_sessionInfo) {}

ServiceStartupInformation::~ServiceStartupInformation() {}

ServiceStartupInformation& ServiceStartupInformation::operator = (const ServiceStartupInformation& _other) {
	m_serviceInfo = _other.m_serviceInfo;
	m_sessionInfo = _other.m_sessionInfo;
	return *this;
}

bool ServiceStartupInformation::operator == (const ServiceStartupInformation& _other) {
	return (m_serviceInfo == _other.m_serviceInfo && m_sessionInfo == _other.m_sessionInfo);
}

bool ServiceStartupInformation::operator != (const ServiceStartupInformation& _other) {
	return !(*this == _other);
}