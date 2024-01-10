#include "ServiceRunStarter.h"
#include "Service.h"
#include "Session.h"
#include "SessionService.h"

#include "OTCore/Logger.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"

#include <thread>

ServiceRunStarter& ServiceRunStarter::instance() {
	static ServiceRunStarter g_instance;
	return g_instance;
}

void ServiceRunStarter::addService(Session * _session, Service * _service) {
	m_mutex.lock();

	startupInformation info;
	
	info.sessionId = _session->id();

	info.serviceId = _service->id();
	info.serviceName = _service->name();
	info.serviceType = _service->type();
	info.serviceUrl = _service->url();

	info.credentialsUserName = _session->getCredentialsUsername();
	info.credentialsUserPassword = _session->getCredentialsPassword();

	info.userCollection = _session->getUserCollection();
	info.sessionType    = _session->getType();

	m_queue.push_back(info);

	m_mutex.unlock();
}

void ServiceRunStarter::sessionClosing(const std::string& _sessionId) {
	m_mutex.lock();
	bool erased = true;
	while (erased) {
		erased = false;
		for (auto it = m_queue.begin(); it != m_queue.end(); it++) {
			if (it->sessionId == _sessionId) {
				m_queue.erase(it);
				erased = true;
				break;
			}
		}
	}
	m_mutex.unlock();
}

ServiceRunStarter::ServiceRunStarter() : m_isStopping(false), m_isRunning(true) {
	std::thread t(&ServiceRunStarter::worker, this);
	t.detach();
}

ServiceRunStarter::~ServiceRunStarter() {
	m_isStopping = true;
}

bool ServiceRunStarter::queueEmpty(void) {
	bool ret = false;
	m_mutex.lock();
	ret = m_queue.empty();
	m_mutex.unlock();
	return ret;
}

void ServiceRunStarter::worker(void) {

	while (!m_isStopping) {
		m_mutex.lock();																			// Lock
		if (m_queue.empty()) {
			m_mutex.unlock();																	// Unlock
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(100ms);
		}
		else {
			// Get next notifier
			startupInformation info = m_queue.front();
			m_queue.pop_front();
			
			// Create run document
			ot::JsonDocument doc;
			doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Run, doc.GetAllocator()), doc.GetAllocator());
			doc.AddMember(OT_PARAM_AUTH_USERNAME, ot::JsonString(info.credentialsUserName, doc.GetAllocator()), doc.GetAllocator());
			doc.AddMember(OT_PARAM_AUTH_PASSWORD, ot::JsonString(info.credentialsUserPassword, doc.GetAllocator()), doc.GetAllocator());
			doc.AddMember(OT_PARAM_SETTINGS_USERCOLLECTION, ot::JsonString(info.userCollection, doc.GetAllocator()), doc.GetAllocator());
			doc.AddMember(OT_ACTION_PARAM_SESSION_TYPE, ot::JsonString(info.sessionType, doc.GetAllocator()), doc.GetAllocator());

			// Get session
			Session * session = SessionService::instance()->getSession(info.sessionId, false);
			if (session == nullptr) {
				m_mutex.unlock();																// Unlock

				OT_LOG_W("Session not found. Cleaning up");
				// Clean up session information
				sessionClosing(info.sessionId);
				continue;
			}
			else {
				session->addServiceListToDocument(doc);

				// Unlock must happen after the addServiceListToDocument() call, since it may happen that the
				// Session Service is deleting the sessing while document is built (racing condition)
				m_mutex.unlock();																// Unlock
			} 
			
			OT_LOG_D("Sending run command to service (name = \"" + info.serviceName + "\"; type = \"" + info.serviceType +
				"\"; id = \"" + std::to_string(info.serviceId) + "\")");

			std::string response;
			std::string messageOut = doc.toJson();
			if (!ot::msg::send("", info.serviceUrl, ot::EXECUTE, messageOut, response, 3000)) {
				assert(0);
				OT_LOG_E("Failed to send run command to service (name = \"" + info.serviceName + "\"; type = \"" + info.serviceType +
					"\"; id = \"" + std::to_string(info.serviceId) + "\")");
				// todo: add error handling for startup error
			}
			if (response != OT_ACTION_RETURN_VALUE_OK) {
				// todo: response should be OK
				//assert(0);
				//OT_LOG_E("Invalid service response (expected: \"" OT_ACTION_RETURN_VALUE_OK "\"): " + response);
			}
		}
	}
	m_isRunning = false;
}