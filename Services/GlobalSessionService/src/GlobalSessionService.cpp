// Global session service header
#include "GlobalSessionService.h"
#include "SessionService.h"
#include "Session.h"

#include "OpenTwinCore/otAssert.h"
#include "OpenTwinCore/Logger.h"
#include "OpenTwinCommunication/Msg.h"

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

bool GlobalSessionService::addSessionService(SessionService& _service) {
	// LOCK
	m_mapMutex.lock();

	// Check URL duplicate
	for (auto it : m_sessionServiceIdMap) {
		if (it.second->url() == _service.url()) {
			OT_LOG_W("Session with the url (" + _service.url() + ") is already registered");
			otAssert(0, "Session url already registered");
			_service.setID(it.second->id());
			// UNLOCK
			m_mapMutex.unlock();
			return true;
		}
	}

	// Create copy and assign uid
	SessionService * newEntry = new SessionService(_service);
	ot::serviceID_t newID = m_sessionServiceIdManager.grabNextID();
	_service.setID(newID);
	newEntry->setID(newID);

	OT_LOG_D("New session service registered @ " + newEntry->url() + " with id=" + std::to_string(newEntry->id()));

	// Register already opened sessions
	for (auto session : newEntry->sessions()) {
		m_sessionToServiceMap.insert_or_assign(session->ID(), newEntry);
		OT_LOG_D("Session service contains already running session with id=" + session->ID());
	}
	m_sessionServiceIdMap.insert_or_assign(newID, newEntry);

	// Check if the health check is aready running, otherwise start it
	if (!m_HealthCheckRunning) {
		m_HealthCheckRunning = true;
		std::thread worker(&GlobalSessionService::healthCheck, this);
		worker.detach();
	}

	// UNLOCK
	m_mapMutex.unlock();

	return true;
}

// ###################################################################################################

// Handler

std::string GlobalSessionService::handleGetDBUrl(OT_rJSON_doc& _doc) {
	return DatabaseURL();
}

std::string GlobalSessionService::handleGetDBandAuthURL(OT_rJSON_doc& _doc) {
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_DBURL, DatabaseURL());
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_AUTHURL, AuthorizationServiceURL());
	return ot::rJSON::toJSON(doc);
}

std::string GlobalSessionService::handleCreateSession(OT_rJSON_doc& _doc) {
	std::string sessionID = ot::rJSON::getString(_doc, OT_ACTION_PARAM_SESSION_ID);
	std::string userName = ot::rJSON::getString(_doc, OT_ACTION_PARAM_USER_NAME);

	m_mapMutex.lock();

	// Check if the session already exists
	auto it = m_sessionToServiceMap.find(sessionID);
	if (it != m_sessionToServiceMap.end()) {

		// Check if the username differs
		if (it->second->getSessionById(sessionID)->UserName() != userName) {
			m_mapMutex.unlock();

			OT_LOG_W("Session opened by other user");
			otAssert(0, "Session open by other user");
			return OT_ACTION_RETURN_INDICATOR_Error "Session open by other user";
		}
		else {
			m_mapMutex.unlock();

			return it->second->url();
		}
	}
	else {
		SessionService * ss = leastLoadedSessionService();
		if (ss) {
			m_mapMutex.unlock();

			OT_LOG_D("Session service @ " + ss->url() + " with id=" + std::to_string(ss->id()) + " provided for session with id=" + sessionID);

			return ss->url();
		}
		else {
			m_mapMutex.unlock();

			OT_LOG_E("No session service connected");
			otAssert(0, "No session service connected");
			return OT_ACTION_RETURN_INDICATOR_Error "No session service connected";
		}
	}
}

std::string GlobalSessionService::handleRegisterSessionService(OT_rJSON_doc& _doc) {
	SessionService nService;
	// Gather information from document
	if (!nService.setFromDocument(_doc)) { return OT_ACTION_RETURN_INDICATOR_Error "Failed to load session service data"; }

	// Add to GSS
	if (!addSessionService(nService)) { return OT_ACTION_RETURN_INDICATOR_Error "Failed to attach service information"; }

	OT_rJSON_createDOC(reply);
	ot::rJSON::add(reply, OT_ACTION_PARAM_SERVICE_ID, nService.id());
	ot::rJSON::add(reply, OT_ACTION_PARAM_DATABASE_URL, m_DatabaseURL);
	ot::rJSON::add(reply, OT_ACTION_PARAM_SERVICE_AUTHURL, m_AuthorizationServiceURL);
	if (!m_GlobalDirectoryServiceURL.empty()) ot::rJSON::add(reply, OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL, m_GlobalDirectoryServiceURL);

	return ot::rJSON::toJSON(reply);
}

std::string GlobalSessionService::handleShutdownSession(OT_rJSON_doc& _doc) {
	std::string sessionID = ot::rJSON::getString(_doc, OT_ACTION_PARAM_SESSION_ID);

	m_mapMutex.lock();

	auto it = m_sessionToServiceMap.find(sessionID);
	if (it != m_sessionToServiceMap.end()) {
		SessionService * ss = it->second;
		ss->removeSession(ss->getSessionById(sessionID));
		m_sessionToServiceMap.erase(sessionID);
	}

	m_mapMutex.unlock();

	OT_LOG_D("Session was closed (ID = \"" + sessionID + "\")");

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string GlobalSessionService::handleCheckProjectOpen(OT_rJSON_doc& _doc) {
	std::string projectName = ot::rJSON::getString(_doc, OT_ACTION_PARAM_PROJECT_NAME);

	m_mapMutex.lock();

	//todo: replace response by a document
	auto it = m_sessionToServiceMap.find(projectName);
	if (it == m_sessionToServiceMap.end()) {
		m_mapMutex.unlock();
		return std::string();
	}
	else {
		m_mapMutex.unlock();
		return it->second->getSessionById(projectName)->UserName();
	}
}

std::string GlobalSessionService::handleForceHealthcheck(OT_rJSON_doc& _doc) {
	if (m_HealthCheckRunning) {
		m_forceHealthCheck = true;
		return OT_ACTION_RETURN_VALUE_OK;
	}
	else return OT_ACTION_RETURN_VALUE_FAILED;
}

std::string GlobalSessionService::handleNewGlobalDirectoryService(OT_rJSON_doc& _doc) {
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
	if (!m_GlobalDirectoryServiceURL.empty()) {
		OT_LOG_D("Removing old Global Directory Service (url = " + m_GlobalDirectoryServiceURL + ")");
	}
	m_GlobalDirectoryServiceURL = _doc[OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL].GetString();

	OT_LOG_D("Set new Global Directory Service (url = " + m_GlobalDirectoryServiceURL + ")");

	// Create notification document for the local session services
	OT_rJSON_createDOC(lssDoc);
	ot::rJSON::add(lssDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_RegisterNewGlobalDirecotoryService);
	ot::rJSON::add(lssDoc, OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL, m_GlobalDirectoryServiceURL);

	// Notify connected Local Session Services
	m_mapMutex.lock();
	std::string response;
	for (auto ss : m_sessionServiceIdMap) {
		if (!ot::msg::send("", ss.second->url(), ot::EXECUTE, ot::rJSON::toJSON(lssDoc), response)) {
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

	OT_rJSON_createDOC(pingDoc);
	ot::rJSON::add(pingDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_Ping);
	std::string pingMessage = ot::rJSON::toJSON(pingDoc);

	while (m_HealthCheckRunning) {
		counter = 0;
		while (counter++ < limit && m_HealthCheckRunning && !m_forceHealthCheck)
		{
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1s);
		}
		m_forceHealthCheck = false;
		if (m_HealthCheckRunning) {
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
						if (!ot::msg::send(m_URL, it.second->url(), ot::EXECUTE, pingMessage, response)) {
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

void GlobalSessionService::removeSessionService(SessionService * _service) {
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

SessionService * GlobalSessionService::leastLoadedSessionService(void) {
	SessionService * leastLoadedSessionService{ nullptr };
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
	m_HealthCheckRunning(false), m_forceHealthCheck(false)
{

}

GlobalSessionService::~GlobalSessionService() {

}