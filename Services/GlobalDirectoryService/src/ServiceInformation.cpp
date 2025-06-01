//! @file ServiceInformation.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

// GDS header
#include "ServiceInformation.h"

ServiceInformation::ServiceInformation() : m_id(ot::invalidServiceID) {}

ServiceInformation::ServiceInformation(const std::string & _name, const std::string & _type, ot::serviceID_t _serviceId, const SessionInformation & _session) :
	m_name(_name), m_type(_type), m_id(_serviceId), m_session(_session)
{}

ServiceInformation::ServiceInformation(const std::string& _name, const std::string& _type, ot::serviceID_t _serviceId, const std::string& _sessionId, const std::string& _sessionServiceURL) :
	m_name(_name), m_type(_type), m_id(_serviceId), m_session(_sessionId, _sessionServiceURL)
{}

ServiceInformation::~ServiceInformation() {}

bool ServiceInformation::operator == (const ServiceInformation& _other) const {
	return m_name == _other.m_name && m_type == _other.m_type && m_id == _other.m_id && m_session == _other.m_session;
}

bool ServiceInformation::operator != (const ServiceInformation& _other) const {
	return !(*this == _other);
}