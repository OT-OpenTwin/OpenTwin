//! @file GlobalSessionService.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2022
// ###########################################################################################################################################################################################################################################################################################################################

// GSS header
#include "GlobalSessionService.h"
#include "LocalSessionService.h"
#include "Session.h"

// OpenTwin header
#include "OTCore/OTAssert.h"
#include "OTCore/Logger.h"
#include "OTCommunication/Msg.h"

// std header
#include <iostream>
#include <thread>
#include <chrono>
#include <limits>

GlobalSessionService * g_instance{ nullptr };

GlobalSessionService * GlobalSessionService::instance(void) {
	if (g_instance == nullptr) g_instance = new GlobalSessionService;
	return g_instance;
}

bool GlobalSessionService::hasInstance(void) {
	return g_instance != nullptr;
}

void GlobalSessionService::deleteInstance(void) {
	if (g_instance) delete g_instance;
	g_instance = nullptr;
}

// ###################################################################################################

// Service handling

bool GlobalSessionService::addSessionService(LocalSessionService& _service) {
	// LOCK
	m_mapMutex.lock();

	// Check URL duplicate
	for (auto it : m_sessionServiceIdMap) {
		if (it.second->url() == _service.url()) {
			OT_LOG_W("Session with the url (" + _service.url() + ") is already registered");
			OTAssert(0, "Session url already registered");
			_service.setId(it.second->id());
			// UNLOCK
			m_mapMutex.unlock();
			return true;
		}
	}

	// Create copy and assign uid
	LocalSessionService* newEntry = new LocalSessionService(_service);
	ot::serviceID_t newID = m_sessionServiceIdManager.grabNextID();
	_service.setId(newID);
	newEntry->setId(newID);

	OT_LOG_D("New session service registered @ " + newEntry->url() + " with id=" + std::to_string(newEntry->id()));

	// Register already opened sessions
	for (auto session : newEntry->sessions()) {
		m_sessionToServiceMap.insert_or_assign(session->id(), newEntry);
		OT_LOG_D("Session service contains already running session with id=" + session->id());
	}
	m_sessionServiceIdMap.insert_or_assign(newID, newEntry);

	// Check if the health check is aready running, otherwise start it
	if (!m_healthCheckRunning) {
		m_healthCheckRunning = true;
		std::thread worker(&GlobalSessionService::healthCheck, this);
		worker.detach();
	}

	// UNLOCK
	m_mapMutex.unlock();

	return true;
}

// ###################################################################################################

// Handler

std::string GlobalSessionService::handleGetDBUrl(ot::JsonDocument& _doc) {
	return m_databaseUrl;
}

std::string GlobalSessionService::handleGetDBandAuthURL(ot::JsonDocument& _doc) {
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_PARAM_SERVICE_DBURL, ot::JsonString(m_databaseUrl, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_AUTHURL, ot::JsonString(m_authorizationUrl, doc.GetAllocator()), doc.GetAllocator());
	return doc.toJson();
}

std::string GlobalSessionService::handleCreateSession(ot::JsonDocument& _doc) {
	std::string sessionID = ot::json::getString(_doc, OT_ACTION_PARAM_SESSION_ID);
	std::string userName = ot::json::getString(_doc, OT_ACTION_PARAM_USER_NAME);

	m_mapMutex.lock();

	// Check if the session already exists
	auto it = m_sessionToServiceMap.find(sessionID);
	if (it != m_sessionToServiceMap.end()) {

		// Check if the username differs
		if (it->second->getSessionById(sessionID)->userName() != userName) {
			m_mapMutex.unlock();

			OT_LOG_W("Session opened by other user");
			OTAssert(0, "Session open by other user");
			return OT_ACTION_RETURN_INDICATOR_Error "Session open by other user";
		}
		else {
			m_mapMutex.unlock();

			return it->second->url();
		}
	}
	else {
		LocalSessionService* ss = leastLoadedSessionService();
		if (ss) {
			m_mapMutex.unlock();

			OT_LOG_D("Session service @ " + ss->url() + " with id=" + std::to_string(ss->id()) + " provided for session with id=" + sessionID);

			return ss->url();
		}
		else {
			m_mapMutex.unlock();

			OT_LOG_E("No session service connected");
			OTAssert(0, "No session service connected");
			return OT_ACTION_RETURN_INDICATOR_Error "No session service connected";
		}
	}
}

std::string GlobalSessionService::handleRegisterSessionService(ot::JsonDocument& _doc) {
	LocalSessionService nService;
	// Gather information from document
	nService.setFromJsonObject(_doc.GetConstObject());

	// Add to GSS
	if (!addSessionService(nService)) { return OT_ACTION_RETURN_INDICATOR_Error "Failed to attach service information"; }

	ot::JsonDocument reply;
	reply.AddMember(OT_ACTION_PARAM_SERVICE_ID, nService.id(), reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_DATABASE_URL, ot::JsonString(m_databaseUrl, reply.GetAllocator()), reply.GetAllocator());
	reply.AddMember(OT_ACTION_PARAM_SERVICE_AUTHURL, ot::JsonString(m_authorizationUrl, reply.GetAllocator()), reply.GetAllocator());
	if (!m_globalDirectoryUrl.empty()) {
		reply.AddMember(OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL, ot::JsonString(m_globalDirectoryUrl, reply.GetAllocator()), reply.GetAllocator());
	}

	return reply.toJson();
}

std::string GlobalSessionService::handleShutdownSession(ot::JsonDocument& _doc) {
	std::string sessionID = ot::json::getString(_doc, OT_ACTION_PARAM_SESSION_ID);

	m_mapMutex.lock();

	auto it = m_sessionToServiceMap.find(sessionID);
	if (it != m_sessionToServiceMap.end()) {
		LocalSessionService* ss = it->second;
		ss->removeSession(ss->getSessionById(sessionID));
		m_sessionToServiceMap.erase(sessionID);
	}

	m_mapMutex.unlock();

	OT_LOG_D("Session was closed (ID = \"" + sessionID + "\")");

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string GlobalSessionService::handleCheckProjectOpen(ot::JsonDocument& _doc) {
	std::string projectName = ot::json::getString(_doc, OT_ACTION_PARAM_PROJECT_NAME);

	m_mapMutex.lock();

	//todo: replace response by a document
	auto it = m_sessionToServiceMap.find(projectName);
	if (it == m_sessionToServiceMap.end()) {
		m_mapMutex.unlock();
		return std::string();
	}
	else {
		m_mapMutex.unlock();
		return it->second->getSessionById(projectName)->userName();
	}
}

std::string GlobalSessionService::handleForceHealthcheck(ot::JsonDocument& _doc) {
	if (m_healthCheckRunning) {
		m_forceHealthCheck = true;
		return OT_ACTION_RETURN_VALUE_OK;
	}
	else return OT_ACTION_RETURN_VALUE_FAILED;
}

std::string GlobalSessionService::handleNewGlobalDirectoryService(ot::JsonDocument& _doc) {
	// Check document
	if (!_doc.HasMember(OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL)) {
		OT_LOG_E("Missing \"" OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL "\" member");
		return OT_ACTION_RETURN_INDICATOR_Error "Missing \"" OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL "\" member";
	}
	if (!_doc[OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL].IsString()) {
		OT_LOG_E("Member \"" OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL "\" is not a string");
		return OT_ACTION_RETURN_INDICATOR_Error "Member \"" OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL "\" is not a string";
	}

	// Set new GDS url
	if (!m_globalDirectoryUrl.empty()) {
		OT_LOG_D("Removing old Global Directory Service (url = " + m_globalDirectoryUrl + ")");
	}
	m_globalDirectoryUrl = _doc[OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL].GetString();

	OT_LOG_D("Set new Global Directory Service (url = " + m_globalDirectoryUrl + ")");

	// Create notification document for the local session services
	ot::JsonDocument lssDoc;
	lssDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_RegisterNewGlobalDirecotoryService, lssDoc.GetAllocator()), lssDoc.GetAllocator());
	lssDoc.AddMember(OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL, ot::JsonString(m_globalDirectoryUrl, lssDoc.GetAllocator()), lssDoc.GetAllocator());

	// Notify connected Local Session Services
	m_mapMutex.lock();
	std::string response;
	for (auto ss : m_sessionServiceIdMap) {
		if (!ot::msg::send("", ss.second->url(), ot::EXECUTE, lssDoc.toJson(), response)) {
			assert(0);
			OT_LOG_E("Failed to send message to Local Session Service (url = " + ss.second->url() + ")");
			m_mapMutex.unlock();
			return OT_ACTION_RETURN_INDICATOR_Error "Failed to send message to Local Session Service (url = " + ss.second->url() + ")";
		}
		if (response != OT_ACTION_RETURN_VALUE_OK) {
			assert(0);
			OT_LOG_E("Invalid response from LSS (url = " + ss.second->url() + "): " + response);
		}
	}
	m_mapMutex.unlock();

	return OT_ACTION_RETURN_VALUE_OK;
}

// ###################################################################################################

// Private functions

void GlobalSessionService::healthCheck() {
	const static int limit{ 60 }; // 1 min
	int counter{ 0 };

	ot::JsonDocument pingDoc;
	pingDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Ping, pingDoc.GetAllocator()), pingDoc.GetAllocator());
	std::string pingMessage = pingDoc.toJson();

	while (m_healthCheckRunning) {
		counter = 0;
		while (counter++ < limit && m_healthCheckRunning && !m_forceHealthCheck)
		{
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1s);
		}
		m_forceHealthCheck = false;
		if (m_healthCheckRunning) {
			m_mapMutex.lock();
			bool running{ true };
			// Use running in case of a disconnected service
			while (running) {
				running = false;
				for (auto it : m_sessionServiceIdMap) {
					
					OT_LOG_D("Performing health check for LSS (url = " + it.second->url() + ")");

					try {
						std::string response;
						// Try to ping
						if (!ot::msg::send(m_url, it.second->url(), ot::EXECUTE, pingMessage, response)) {
							OT_LOG_W("Ping could not be delivered to LSS (url = " + it.second->url() + ")");
							removeSessionService(it.second);
							running = true;
							break;
						}
						// Check ping response
						if (response != OT_ACTION_CMD_Ping) {
							OT_LOG_W("Invalid ping response from LSS (url = " + it.second->url() + ")");
							removeSessionService(it.second);
							running = true;
							break;
						}
					}
					catch (const std::exception& e) {
						OT_LOG_E("Health check for LSS (url = " + it.second->url() + ") failed: " + e.what());
						removeSessionService(it.second);
						running = true;
						break;
					}
					catch (...) {
						OT_LOG_E("[FATAL] Health check for LSS (url = " + it.second->url() + ") failed: Unknown error");
						removeSessionService(it.second);
						running = true;
						break;
					}
				}
			}
			m_mapMutex.unlock();
		}
	}
}

void GlobalSessionService::removeSessionService(LocalSessionService * _service) {
	if (_service) {
		// Service UID map
		m_sessionServiceIdMap.erase(_service->id());

		// Session to Service map
		bool erased{ true };
		while (erased) {
			erased = false;
			for (auto it : m_sessionToServiceMap) {
				if (it.second == _service) {
					erased = true;
					m_sessionToServiceMap.erase(it.first);
					break;
				}
			}
		}
		delete _service;
	}
}

LocalSessionService * GlobalSessionService::leastLoadedSessionService(void) {
	LocalSessionService * leastLoadedSessionService{ nullptr };
	size_t minLoad{ UINT64_MAX };
	for (auto it : m_sessionServiceIdMap) {
		if (it.second->sessionCount() < minLoad) {
			leastLoadedSessionService = it.second;
			minLoad = leastLoadedSessionService->sessionCount();
		}
	}
	return leastLoadedSessionService;
}

GlobalSessionService::GlobalSessionService()
	: ot::ServiceBase(OT_INFO_SERVICE_TYPE_GlobalSessionService, OT_INFO_SERVICE_TYPE_GlobalSessionService),
	m_healthCheckRunning(false), m_forceHealthCheck(false)
{

}

GlobalSessionService::~GlobalSessionService() {

}