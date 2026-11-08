// @otlicense

#pragma once

// Auth header
#include "SSO/SSOUser.h"
#include "SSO/SSOAuthenticationProcessID.h"

// OpenTwin header
#include "OTCore/JSON/JSON.h"
#include "OTSystem/SingleSignOn/SingleSignOn_Server.h"

// std header
#include <map>
#include <memory>
#include <string>
#include <unordered_map>

class SSOBuffer
{
public:
	bool handleRequest(std::string& _determinedUsername, const std::string& _authenticationProcessID, const std::string& _receivedToken, bool _initialToken, ot::JsonDocument& _returnMessage);
	SSOUser* getUser (const std::string& _token);
	void removeUser(const std::string& _token);
	void clearExpiredProcessIDs();
	bool validate(const std::string& _token);
private:
	//! @brief Here we keep the current state of the SSO authentication process per username.
	std::unordered_map<ot::SSOAuthenticationProcessID, std::unique_ptr<ot::SingleSignOn_Server>> m_serverSignOnByProcessID;

	//! @brief Here we buffer the logged in sso users
	std::map<std::string, SSOUser> m_loggedInSSOUsersByToken;
};
