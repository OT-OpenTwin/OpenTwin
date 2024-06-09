//! @file LoginData.h
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Frontend header
#include "LogInGSSEntry.h"

// std header
#include <string>

class LoginData {
public:
	LoginData() {};
	LoginData(const LoginData& _other);
	~LoginData() {};

	LoginData& operator = (const LoginData& _other);

	void setGss(const LogInGSSEntry& _gss) { m_gss = _gss; };
	const LogInGSSEntry& getGss(void) const { return m_gss; };

	void setDatabaseUrl(const std::string& _url) { m_databaseUrl = _url; };
	const std::string& getDatabaseUrl(void) const { return m_databaseUrl; };

	void setAuthorizationUrl(const std::string& _url) { m_authorizationUrl = _url; };
	const std::string& getAuthorizationUrl(void) const { return m_authorizationUrl; };

	void setUserName(const std::string& _name) { m_username = _name; };
	const std::string getUserName(void) const { return m_username; };

	void setUserPassword(const std::string& _clearPassword) { m_userPassword = _clearPassword; };
	const std::string& getUserPassword(void) const { return m_userPassword; };

	void setEncryptedUserPassword(const std::string& _encryptedPassword) { m_encryptedUserPassword = _encryptedPassword; };
	const std::string& getEncryptedUserPassword(void) const { return m_encryptedUserPassword; };

	void setSessionUser(const std::string& _sessionUser) { m_sessionUser = _sessionUser; };
	const std::string& getSessionUser(void) const { return m_sessionUser; };

	void setSessionPassword(const std::string& _sessionPassword) { m_sessionPassword = _sessionPassword; };
	const std::string& getSessionPassword(void) const { return m_sessionPassword; };

	void clear(void);

	bool isValid(void) const;

private:
	LogInGSSEntry m_gss;

	std::string m_databaseUrl;
	std::string m_authorizationUrl;
	std::string m_username;
	std::string m_userPassword;
	std::string m_encryptedUserPassword;
	std::string m_sessionUser;
	std::string m_sessionPassword;

};