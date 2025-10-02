//! @file GSSDebugInfo.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCommunication/GSSDebugInfo.h"

// ###########################################################################################################################################################################################################################################################################################################################

// Virtual methods

void ot::GSSDebugInfo::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const {
	_jsonObject.AddMember("DatabaseUrl", JsonString(m_databaseUrl, _allocator), _allocator);
	_jsonObject.AddMember("AuthorizationUrl", JsonString(m_authorizationUrl, _allocator), _allocator);
	_jsonObject.AddMember("GlobalDirectoryUrl", JsonString(m_globalDirectoryUrl, _allocator), _allocator);
	_jsonObject.AddMember("LibraryManagementUrl", JsonString(m_libraryManagementUrl, _allocator), _allocator);

	_jsonObject.AddMember("WorkerRunning", m_workerRunning, _allocator);

	JsonArray sessionToLssArr;
	for (const auto& it : m_sessionToLssList) {
		JsonObject lssObj;
		lssObj.AddMember("SessionID", JsonString(it.first, _allocator), _allocator);
		lssObj.AddMember("LssID", it.second, _allocator);
		sessionToLssArr.PushBack(lssObj, _allocator);
	}
	_jsonObject.AddMember("SessionToLssList", sessionToLssArr, _allocator);

	JsonArray lssArr;
	for (const LSSData& lss : m_lssList) {
		JsonObject lssObj;
		this->addToJsonObject(lssObj, _allocator, lss);
		lssArr.PushBack(lssObj, _allocator);
	}
	_jsonObject.AddMember("LssList", lssArr, _allocator);
}

void ot::GSSDebugInfo::setFromJsonObject(const ConstJsonObject& _jsonObject) {
	m_databaseUrl = json::getString(_jsonObject, "DatabaseUrl");
	m_authorizationUrl = json::getString(_jsonObject, "AuthorizationUrl");
	m_globalDirectoryUrl = json::getString(_jsonObject, "GlobalDirectoryUrl");
	m_libraryManagementUrl = json::getString(_jsonObject, "LibraryManagementUrl");

	m_workerRunning = json::getBool(_jsonObject, "WorkerRunning");

	m_sessionToLssList.clear();
	for (const ConstJsonObject& sessionObj : json::getObjectList(_jsonObject, "SessionToLssList")) {
		std::string sessionID = json::getString(sessionObj, "SessionID");
		ot::serviceID_t lssID = json::getUInt(sessionObj, "LssID");
		m_sessionToLssList.push_back(std::make_pair(sessionID, lssID));
	}

	m_lssList.clear();
	for (const ConstJsonObject& lssObj : json::getObjectList(_jsonObject, "LssList")) {
		LSSData lss;
		this->setFromJsonObject(lssObj, lss);
		m_lssList.push_back(std::move(lss));
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

void ot::GSSDebugInfo::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator, const LSSData& _data) const {
	_jsonObject.AddMember("ID", JsonNumber(_data.id), _allocator);
	_jsonObject.AddMember("Url", JsonString(_data.url, _allocator), _allocator);

	JsonArray activeArr;
	for (const SessionData& session : _data.activeSessions) {
		JsonObject sessionObj;
		this->addToJsonObject(sessionObj, _allocator, session);
		activeArr.PushBack(sessionObj, _allocator);
	}
	_jsonObject.AddMember("ActiveSessions", activeArr, _allocator);

	JsonArray initArr;
	for (const SessionData& session : _data.initializingSessions) {
		JsonObject sessionObj;
		this->addToJsonObject(sessionObj, _allocator, session);
		initArr.PushBack(sessionObj, _allocator);
	}
	_jsonObject.AddMember("InitializingSessions", initArr, _allocator);
}

void ot::GSSDebugInfo::setFromJsonObject(const ConstJsonObject& _jsonObject, LSSData& _data) {
	_data.id = json::getUInt(_jsonObject, "ID");
	_data.url = json::getString(_jsonObject, "Url");

	_data.activeSessions.clear();
	for (const ConstJsonObject& sessionObj : json::getObjectList(_jsonObject, "ActiveSessions")) {
		SessionData session;
		this->setFromJsonObject(sessionObj, session);
		_data.activeSessions.push_back(std::move(session));
	}

	_data.initializingSessions.clear();
	for (const ConstJsonObject& sessionObj : json::getObjectList(_jsonObject, "InitializingSessions")) {
		SessionData session;
		this->setFromJsonObject(sessionObj, session);
		_data.initializingSessions.push_back(std::move(session));
	}
}

void ot::GSSDebugInfo::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator, const SessionData& _data) const {
	_jsonObject.AddMember("SessionID", JsonString(_data.sessionID, _allocator), _allocator);
	_jsonObject.AddMember("UserName", JsonString(_data.userName, _allocator), _allocator);
	_jsonObject.AddMember("Flags", JsonArray(_data.flags, _allocator), _allocator);
}

void ot::GSSDebugInfo::setFromJsonObject(const ConstJsonObject& _jsonObject, SessionData& _data) {
	_data.sessionID = json::getString(_jsonObject, "SessionID");
	_data.userName = json::getString(_jsonObject, "UserName");
	_data.flags = json::getStringList(_jsonObject, "Flags");
}
