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
	UserSession(const std::string& _sessionName, const std::string& _token, int64_t _validityInSeconds);

	bool isValid() const;
	void updateExpirationTime(int64_t _validityInSeconds);
	
	const std::string& getName() const { return m_name; };
	const std::string& getToken() const { return m_token; };

	void setLoggedInUserName(const std::string& _loggedInUserName) { m_loggedInUserName = _loggedInUserName; };
	const std::string& getLoggedInUserName() const { return m_loggedInUserName; };

	void setSessionUserName(const std::string& _sessionUserName) { m_sessionUserName = _sessionUserName; };
	const std::string& getSessionUserName() const { return m_sessionUserName; };

	void setSessionUserPWD(const std::string& _sessionUserPWD) { m_sessionUserPWD = _sessionUserPWD; };
	const std::string& getSessionUserPWD() const { return m_sessionUserPWD; };

private:
	std::string m_name;
	std::string m_token;

	std::string m_loggedInUserName;
	std::string m_sessionUserName;
	std::string m_sessionUserPWD;
	int64_t m_expirationTime;
};