//! @file ServiceInitData.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCommunication/ServiceInitData.h"

ot::ServiceInitData::ServiceInitData() : m_serviceID(ot::invalidServiceID) {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Virtual methods

void ot::ServiceInitData::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const {
	_jsonObject.AddMember("ServiceID", m_serviceID, _allocator);

	_jsonObject.AddMember("SessionServiceURL", JsonString(m_sessionServiceURL, _allocator), _allocator);
	_jsonObject.AddMember("SessionID", JsonString(m_sessionID, _allocator), _allocator);
	_jsonObject.AddMember("SessionType", JsonString(m_sessionType, _allocator), _allocator);

	_jsonObject.AddMember("Username", JsonString(m_username, _allocator), _allocator);
	_jsonObject.AddMember("Password", JsonString(m_password, _allocator), _allocator);
	_jsonObject.AddMember("DatabaseUsername", JsonString(m_databaseUsername, _allocator), _allocator);
	_jsonObject.AddMember("DatabasePassword", JsonString(m_databasePassword, _allocator), _allocator);
	_jsonObject.AddMember("UserCollection", JsonString(m_userCollection, _allocator), _allocator);
}

void ot::ServiceInitData::setFromJsonObject(const ConstJsonObject& _jsonObject) {
	m_serviceID = static_cast<serviceID_t>(json::getUInt(_jsonObject, "ServiceID"));

	m_sessionServiceURL = json::getString(_jsonObject, "SessionServiceURL");
	m_sessionID = json::getString(_jsonObject, "SessionID");
	m_sessionType = json::getString(_jsonObject, "SessionType");

	m_username = json::getString(_jsonObject, "Username");
	m_password = json::getString(_jsonObject, "Password");
	m_databaseUsername = json::getString(_jsonObject, "DatabaseUsername");
	m_databasePassword = json::getString(_jsonObject, "DatabasePassword");
	m_userCollection = json::getString(_jsonObject, "UserCollection");
}
