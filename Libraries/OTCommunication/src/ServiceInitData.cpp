// @otlicense
// File: ServiceInitData.cpp
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

// OpenTwin header
#include "OTCommunication/ServiceInitData.h"

ot::ServiceInitData::ServiceInitData() : m_serviceID(ot::invalidServiceID) {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Virtual methods

void ot::ServiceInitData::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const {
	JsonArray flagsArr;
	addLogFlagsToJsonArray(m_logFlags, flagsArr, _allocator);
	_jsonObject.AddMember("LogFlags", flagsArr, _allocator);
	_jsonObject.AddMember("LoggerUrl", JsonString(m_loggerUrl, _allocator), _allocator);

	_jsonObject.AddMember("Name", JsonString(m_name, _allocator), _allocator);
	_jsonObject.AddMember("Type", JsonString(m_type, _allocator), _allocator);
	_jsonObject.AddMember("ServiceID", m_serviceID, _allocator);

	_jsonObject.AddMember("SessionServiceURL", JsonString(m_sessionServiceURL, _allocator), _allocator);
	_jsonObject.AddMember("SessionID", JsonString(m_sessionID, _allocator), _allocator);
	_jsonObject.AddMember("SessionType", JsonString(m_sessionType, _allocator), _allocator);

	_jsonObject.AddMember("AuthUrl", JsonString(m_authUrl, _allocator), _allocator);
	_jsonObject.AddMember("LMSUrl", JsonString(m_lmsUrl, _allocator), _allocator);

	_jsonObject.AddMember("DatabaseUrl", JsonString(m_databaseUrl, _allocator), _allocator);
	_jsonObject.AddMember("Username", JsonString(m_username, _allocator), _allocator);
	_jsonObject.AddMember("Password", JsonString(m_password, _allocator), _allocator);
	_jsonObject.AddMember("DatabaseUsername", JsonString(m_databaseUsername, _allocator), _allocator);
	_jsonObject.AddMember("DatabasePassword", JsonString(m_databasePassword, _allocator), _allocator);
	_jsonObject.AddMember("UserCollection", JsonString(m_userCollection, _allocator), _allocator);
}

void ot::ServiceInitData::setFromJsonObject(const ConstJsonObject& _jsonObject) {
	m_loggerUrl = json::getString(_jsonObject, "LoggerUrl");
	m_logFlags = logFlagsFromJsonArray(json::getArray(_jsonObject, "LogFlags"));

	m_name = json::getString(_jsonObject, "Name");
	m_type = json::getString(_jsonObject, "Type");
	m_serviceID = static_cast<serviceID_t>(json::getUInt(_jsonObject, "ServiceID"));

	m_sessionServiceURL = json::getString(_jsonObject, "SessionServiceURL");
	m_sessionID = json::getString(_jsonObject, "SessionID");
	m_sessionType = json::getString(_jsonObject, "SessionType");

	m_authUrl = json::getString(_jsonObject, "AuthUrl");
	m_lmsUrl = json::getString(_jsonObject, "LMSUrl");

	m_databaseUrl = json::getString(_jsonObject, "DatabaseUrl");
	m_username = json::getString(_jsonObject, "Username");
	m_password = json::getString(_jsonObject, "Password");
	m_databaseUsername = json::getString(_jsonObject, "DatabaseUsername");
	m_databasePassword = json::getString(_jsonObject, "DatabasePassword");
	m_userCollection = json::getString(_jsonObject, "UserCollection");
}
