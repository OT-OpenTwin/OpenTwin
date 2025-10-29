// @otlicense

//! @file LoginData.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

// Frontend header
#include "LoginData.h"

void LoginData::addRequiredDataToJson(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) const {
	_jsonObject.AddMember("UserName", ot::JsonString(m_username, _allocator), _allocator);
	_jsonObject.AddMember("UserPassword", ot::JsonString(m_encryptedUserPassword, _allocator), _allocator);
	_jsonObject.AddMember("GSS", ot::JsonString(m_gss.getUrl().toStdString(), _allocator), _allocator);
}

void LoginData::setFromRequiredDataJson(const ot::ConstJsonObject& _jsonObject) {
	m_username = ot::json::getString(_jsonObject, "UserName");
	m_encryptedUserPassword = ot::json::getString(_jsonObject, "UserPassword");
	m_gss.setUrl(QString::fromStdString(ot::json::getString(_jsonObject, "GSS")));
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