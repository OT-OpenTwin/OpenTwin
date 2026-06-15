#pragma once
#include <string>
#include <map>
#include <memory>
#include "OTSystem/SingleSignOn/SingleSignOn_Server.h"
#include "SSOUser.h"
class SSOBuffer
{
public:
	bool handleRequest(const std::string& _username, const std::string& _receivedToken, bool _initialToken, std::string& _returnMessage);
	std::optional<std::string> getToken(const std::string& _userName);
	
private:
	//! @brief Here we keep the current state of the SSO authentication process per username.
	std::map<std::string, std::unique_ptr<ot::SingleSignOn_Server>> m_serverSignOnByUsername;

	//! @brief Here we buffer the logged in sso users
	std::map < std::string, SSOUser> m_loggedInSSOUsersByUsername;
};
