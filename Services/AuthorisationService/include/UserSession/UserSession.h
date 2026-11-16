// @otlicense

#pragma once

// Auth header
#include "Management/User.h"

class UserSession
{
	OT_DECL_NOCOPY(UserSession)
	OT_DECL_DEFMOVE(UserSession)
	OT_DECL_NODEFAULT(UserSession)
public:
	UserSession(const User& _user, int64_t _validityInSeconds);

	const User& getUser() const { return m_user; };

	bool isValid() const;
	void updateExpirationTime(int64_t _validityInSeconds);
	
	const std::string& getToken() const { return m_token; };

private:
	std::string m_token;
	User m_user;
	int64_t m_expirationTime;
};