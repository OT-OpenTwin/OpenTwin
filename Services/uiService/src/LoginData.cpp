// @otlicense
// File: LoginData.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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