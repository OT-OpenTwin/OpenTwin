// @otlicense

// Auth header
#include "UserSession/UserSession.h"

// OpenTwin header
#include "OTSystem/DateTime.h"

UserSession::UserSession(const User& _user, int64_t _validityInSeconds)
	: m_user(_user), m_expirationTime(ot::DateTime::msSinceEpoch() + (_validityInSeconds * 1000))
{}

bool UserSession::isValid() const
{
	int64_t currentTime = ot::DateTime::msSinceEpoch();
	return currentTime < m_expirationTime
		&& !m_user.getName().empty();
}

void UserSession::updateExpirationTime(int64_t _validityInSeconds)
{
	m_expirationTime = ot::DateTime::msSinceEpoch() + (_validityInSeconds * 1000);
}