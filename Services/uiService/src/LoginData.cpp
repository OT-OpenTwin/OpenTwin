//! @file LoginData.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

// Frontend header
#include "LoginData.h"

LoginData::LoginData(const LoginData& _other) {
	*this = _other;
}

LoginData& LoginData::operator = (const LoginData& _other) {
	if (this == &_other) return *this;

	m_gss = _other.m_gss;
	m_databaseUrl = _other.m_databaseUrl;
	m_authorizationUrl = _other.m_authorizationUrl;
	m_username = _other.m_username;
	m_userPassword = _other.m_userPassword;
	m_encryptedUserPassword = _other.m_encryptedUserPassword;
	m_sessionUser = _other.m_sessionUser;
	m_sessionPassword = _other.m_sessionPassword;

	return *this;
}

void LoginData::clear(void) {
	m_gss.clear();
	m_databaseUrl.clear();
	m_authorizationUrl.clear();
	m_username.clear();
	m_userPassword.clear();
	m_encryptedUserPassword.clear();
	m_sessionUser.clear();
	m_sessionPassword.clear();
}

bool LoginData::isValid(void) const {
	return m_gss.isValid() &&
		!m_databaseUrl.empty() &&
		!m_authorizationUrl.empty() &&
		!m_username.empty() &&
		!m_userPassword.empty() &&
		!m_encryptedUserPassword.empty() &&
		!m_sessionUser.empty() &&
		!m_sessionPassword.empty();
}