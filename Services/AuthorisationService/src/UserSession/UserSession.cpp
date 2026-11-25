// @otlicense

// Auth header
#include "UserSession/UserSession.h"

// OpenTwin header
#include "OTSystem/DateTime.h"

UserSession::UserSession(const std::string& _sessionName, const std::string& _token, int64_t _validityInSeconds)
	: m_name(_sessionName), m_token(_token), m_expirationTime(ot::DateTime::msSinceEpoch() + (_validityInSeconds * 1000))
{}

bool UserSession::isExpired() const
{
	int64_t currentTime = ot::DateTime::msSinceEpoch();
	return currentTime >= m_expirationTime;
}

void UserSession::updateExpirationTime(int64_t _validityInSeconds)
{
	m_expirationTime = ot::DateTime::msSinceEpoch() + (_validityInSeconds * 1000);
}