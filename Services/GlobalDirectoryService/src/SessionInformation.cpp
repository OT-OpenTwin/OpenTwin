// Project header
#include "SessionInformation.h"

SessionInformation::SessionInformation() {}

SessionInformation::SessionInformation(const std::string& _id, const std::string& _sessionServiceURL)
	: m_id(_id), m_sessionServiceURL(_sessionServiceURL)
{}

SessionInformation::SessionInformation(const SessionInformation& _other)
	: m_id(_other.m_id), m_sessionServiceURL(_other.m_sessionServiceURL)
{}

SessionInformation::~SessionInformation() {}

SessionInformation& SessionInformation::operator = (const SessionInformation& _other) {
	m_id = _other.m_id;
	m_sessionServiceURL = _other.m_sessionServiceURL;
	return *this;
}

bool SessionInformation::operator == (const SessionInformation& _other) const {
	return m_id == _other.m_id && m_sessionServiceURL == _other.m_sessionServiceURL;
}

bool SessionInformation::operator != (const SessionInformation& _other) const {
	return !(*this == _other);
}