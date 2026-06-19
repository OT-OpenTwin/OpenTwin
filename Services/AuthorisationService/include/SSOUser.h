#pragma once
#include "OTSystem/SingleSignOn/LogInInfos.h"
#include "SessionToken.h"
class SSOUser
{
public: 
	//! @brief Generates a session token for the user and stores the login information.
	SSOUser(const ot::LogInInfos& _logInInfos);
	SSOUser(const SSOUser& _user) = delete;
	SSOUser(SSOUser&& _user) = default;
	SSOUser& operator=(const SSOUser& _user) = delete;
	SSOUser& operator=(SSOUser&& _user) = default;

	ot::SessionToken& getSessionToken() { return m_sessionToken; }
	std::string getUserName() const { return m_logInInfos.m_userName; }
private:
	ot::LogInInfos m_logInInfos;
	ot::SessionToken m_sessionToken;
};
