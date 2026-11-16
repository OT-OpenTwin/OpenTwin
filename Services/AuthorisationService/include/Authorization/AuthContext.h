// @otlicense

#pragma once

// Auth header
#include "AccessToken/AccessTokenManager.h"
#include "SSO/SSOBuffer.h"
#include "Ticket/TicketManager.h"
#include "UserSession/UserSessionManager.h"

class AuthContext
{
	OT_DECL_NOCOPY(AuthContext)
	OT_DECL_NOMOVE(AuthContext)
public:
	explicit AuthContext();
	~AuthContext() = default;
	
	AccessTokenManager& getAccessTokenManager() { return m_accessTokenManager; };
	const AccessTokenManager& getAccessTokenManager() const { return m_accessTokenManager; };
	
	SSOBuffer& getSSOBuffer() { return m_ssoBuffer; };
	const SSOBuffer& getSSOBuffer() const { return m_ssoBuffer; };

	TicketManager& getTicketManager() { return m_ticketManager; };
	const TicketManager& getTicketManager() const { return m_ticketManager; };

	UserSessionManager& getUserSessionManager() { return m_userSessionManager; };
	const UserSessionManager& getUserSessionManager() const { return m_userSessionManager; };

private:
	AccessTokenManager m_accessTokenManager;
	SSOBuffer m_ssoBuffer;
	TicketManager m_ticketManager;
	UserSessionManager m_userSessionManager;
};