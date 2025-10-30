// @otlicense
// File: LSSDebugInfo.cpp
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
#include "OTCommunication/LSSDebugInfo.h"

// ###########################################################################################################################################################################################################################################################################################################################

// Virtual methods

void ot::LSSDebugInfo::addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const {
	_jsonObject.AddMember("IsWorkerRunning", m_workerRunning, _allocator);

	_jsonObject.AddMember("Url", JsonString(m_url, _allocator), _allocator);
	_jsonObject.AddMember("ID", m_ID, _allocator);

	_jsonObject.AddMember("GSSUrl", JsonString(m_gssUrl, _allocator), _allocator);
	_jsonObject.AddMember("GSSConnected", m_gssConnected, _allocator);

	_jsonObject.AddMember("GDSUrl", JsonString(m_gdsUrl, _allocator), _allocator);
	_jsonObject.AddMember("GDSConnected", m_gdsConnected, _allocator);

	_jsonObject.AddMember("AuthUrl", JsonString(m_authUrl, _allocator), _allocator);
	_jsonObject.AddMember("DBUrl", JsonString(m_dataBaseUrl, _allocator), _allocator);
	_jsonObject.AddMember("LMSUrl", JsonString(m_lmsUrl, _allocator), _allocator);

	_jsonObject.AddMember("DebugServices", JsonArray(m_debugServices, _allocator), _allocator);

	JsonArray mandatoryArr;
	for (const auto& it : m_mandatoryServices) {
		JsonObject mandObj;
		mandObj.AddMember("SessionType", JsonString(it.first, _allocator), _allocator);
		mandObj.AddMember("Services", JsonArray(it.second, _allocator), _allocator);
		mandatoryArr.PushBack(mandObj, _allocator);
	}
	_jsonObject.AddMember("MandatoryServices", mandatoryArr, _allocator);

	JsonArray shutdownQueueArr;
	for (const auto& it : m_shutdownQueue) {
		JsonObject shutObj;
		shutObj.AddMember("Session", JsonString(it.first, _allocator), _allocator);
		shutObj.AddMember("IsEmergency", it.second, _allocator);
		shutdownQueueArr.PushBack(shutObj, _allocator);
	}
	_jsonObject.AddMember("ShutdownQueue", shutdownQueueArr, _allocator);
	_jsonObject.AddMember("ShutdownCompletedQueue", JsonArray(m_shutdownCompletedQueue, _allocator), _allocator);

	_jsonObject.AddMember("BlockedPorts", JsonArray(m_blockedPorts, _allocator), _allocator);

	JsonArray sessionArr;
	for (const auto& it : m_sessions) {
		JsonObject sessObj;
		sessObj.AddMember("SessionID", JsonString(it.id, _allocator), _allocator);
		sessObj.AddMember("SessionType", JsonString(it.type, _allocator), _allocator);
		sessObj.AddMember("UserName", JsonString(it.userName, _allocator), _allocator);
		sessObj.AddMember("ProjectName", JsonString(it.projectName, _allocator), _allocator);
		sessObj.AddMember("CollectionName", JsonString(it.collectionName, _allocator), _allocator);

		sessObj.AddMember("UserCollection", JsonString(it.userCollection, _allocator), _allocator);
		sessObj.AddMember("UserCredName", JsonString(it.userCredName, _allocator), _allocator);
		sessObj.AddMember("UserCredPassword", JsonString(it.userCredPassword, _allocator), _allocator);
		sessObj.AddMember("DataBaseCredName", JsonString(it.dataBaseCredName, _allocator), _allocator);
		sessObj.AddMember("DataBaseCredPassword", JsonString(it.dataBaseCredPassword, _allocator), _allocator);

		sessObj.AddMember("IsHealthCheckRunning", it.isHealthCheckRunning, _allocator);
		sessObj.AddMember("IsShuttingDown", it.isShuttingDown, _allocator);

		JsonArray serviceArr;
		for (const auto& s : it.services) {
			JsonObject servObj;
			servObj.AddMember("ID", s.id, _allocator);
			servObj.AddMember("Name", JsonString(s.name, _allocator), _allocator);
			servObj.AddMember("Type", JsonString(s.type, _allocator), _allocator);
			servObj.AddMember("URL", JsonString(s.url, _allocator), _allocator);
			servObj.AddMember("WebsocketUrl", JsonString(s.websocketUrl, _allocator), _allocator);
			servObj.AddMember("ConnectedType", JsonString(s.connectedType, _allocator), _allocator);

			servObj.AddMember("IsDebug", s.isDebug, _allocator);
			servObj.AddMember("IsRequested", s.isRequested, _allocator);
			servObj.AddMember("IsAlive", s.isAlive, _allocator);
			servObj.AddMember("IsRunning", s.isRunning, _allocator);
			servObj.AddMember("IsShuttingDown", s.isShuttingDown, _allocator);
			servObj.AddMember("IsHidden", s.isHidden, _allocator);

			serviceArr.PushBack(servObj, _allocator);
		}
		sessObj.AddMember("Services", serviceArr, _allocator);

		sessionArr.PushBack(sessObj, _allocator);
	}
	_jsonObject.AddMember("Sessions", sessionArr, _allocator);
}

void ot::LSSDebugInfo::setFromJsonObject(const ConstJsonObject& _jsonObject) {
	m_workerRunning = json::getBool(_jsonObject, "IsWorkerRunning");

	m_url = json::getString(_jsonObject, "Url");
	m_ID = json::getUInt(_jsonObject, "ID");

	m_gssUrl = json::getString(_jsonObject, "GSSUrl");
	m_gssConnected = json::getBool(_jsonObject, "GSSConnected");

	m_gdsUrl = json::getString(_jsonObject, "GDSUrl");
	m_gdsConnected = json::getBool(_jsonObject, "GDSConnected");

	m_authUrl = json::getString(_jsonObject, "AuthUrl");
	m_dataBaseUrl = json::getString(_jsonObject, "DBUrl");
	m_lmsUrl = json::getString(_jsonObject, "LMSUrl");

	m_debugServices = json::getStringList(_jsonObject, "DebugServices");

	m_mandatoryServices.clear();
	for (const ConstJsonObject& it : json::getObjectList(_jsonObject, "MandatoryServices")) {
		std::string sessionType = json::getString(it, "SessionType");
		std::list<std::string> services = json::getStringList(it, "Services");
		m_mandatoryServices.push_back(std::make_pair(sessionType, services));
	}

	m_shutdownQueue.clear();
	for (const ConstJsonObject& it : json::getObjectList(_jsonObject, "ShutdownQueue")) {
		std::string session = json::getString(it, "Session");
		bool isEmergency = json::getBool(it, "IsEmergency");
		m_shutdownQueue.push_back(std::make_pair(session, isEmergency));
	}

	m_shutdownCompletedQueue = json::getStringList(_jsonObject, "ShutdownCompletedQueue");

	m_blockedPorts.clear();
	for (uint32_t port : json::getUIntList(_jsonObject, "BlockedPorts")) {
		m_blockedPorts.push_back(static_cast<port_t>(port));
	}
	
	m_sessions.clear();
	for (const ConstJsonObject& it : json::getObjectList(_jsonObject, "Sessions")) {
		SessionInfo session;
		session.id = json::getString(it, "SessionID");
		session.type = json::getString(it, "SessionType");
		session.userName = json::getString(it, "UserName");
		session.projectName = json::getString(it, "ProjectName");
		session.collectionName = json::getString(it, "CollectionName");
		session.userCollection = json::getString(it, "UserCollection");
		session.userCredName = json::getString(it, "UserCredName");
		session.userCredPassword = json::getString(it, "UserCredPassword");
		session.dataBaseCredName = json::getString(it, "DataBaseCredName");
		session.dataBaseCredPassword = json::getString(it, "DataBaseCredPassword");

		session.isHealthCheckRunning = json::getBool(it, "IsHealthCheckRunning");
		session.isShuttingDown = json::getBool(it, "IsShuttingDown");

		for (const ConstJsonObject& s : json::getObjectList(it, "Services")) {
			ServiceInfo service;
			service.id = json::getUInt(s, "ID");
			service.name = json::getString(s, "Name");
			service.type = json::getString(s, "Type");
			service.url = json::getString(s, "URL");
			service.websocketUrl = json::getString(s, "WebsocketUrl");
			service.connectedType = json::getString(s, "ConnectedType");
			service.isDebug = json::getBool(s, "IsDebug");
			service.isRequested = json::getBool(s, "IsRequested");
			service.isAlive = json::getBool(s, "IsAlive");
			service.isRunning = json::getBool(s, "IsRunning");
			service.isShuttingDown = json::getBool(s, "IsShuttingDown");
			service.isHidden = json::getBool(s, "IsHidden");
			session.services.push_back(service);
		}
		m_sessions.push_back(session);
	}
		
}