#include "StartupDispatcher.h"
#include "Application.h"
#include "LocalDirectoryService.h"

#include "OTCore/Logger.h"
#include "OTCommunication/Msg.h"

#include <thread>
#include <iostream>

StartupDispatcher::StartupDispatcher(void) : m_isStopping(false),
	m_workerThread(nullptr)
{

}

StartupDispatcher::~StartupDispatcher() {
	// todo: add cleanup
}

// ####################################################################################################################

// Request management

void StartupDispatcher::addRequest(const ServiceStartupInformation& _info) {
	m_mutex.lock();
	m_requests.push_back(_info);
	run();
	m_mutex.unlock();
}

void StartupDispatcher::addRequest(const std::list<ServiceStartupInformation>& _info) {
	m_mutex.lock();
	for (auto r : _info) m_requests.push_back(r);
	run();
	m_mutex.unlock();
}

// ####################################################################################################################

void StartupDispatcher::run(void) {
	if (m_workerThread) return;
	m_isStopping = false;
	m_workerThread = new std::thread(&StartupDispatcher::workerFunction, this);
}

void StartupDispatcher::stop(void) {
	if (m_workerThread == nullptr) return;
	m_isStopping = true;
	m_workerThread->join();
	delete m_workerThread;
	m_workerThread = nullptr;
}

// ####################################################################################################################

// Private functions

void StartupDispatcher::workerFunction(void) {
	using namespace std::chrono_literals;

	while (!m_isStopping) {
		// Check if there are any queued requests
		while (!m_isStopping) {
			m_mutex.lock();
			if (!m_requests.empty()) {
				m_mutex.unlock();
				break;
			}
			m_mutex.unlock();

			std::this_thread::sleep_for(100ms);
		}

		// Dispatch queued requests
		while (!m_isStopping) {
			m_mutex.lock();
			if (m_requests.empty()) {
				m_mutex.unlock();
				break;
			}
			
			// Grab first item
			ServiceStartupInformation info = m_requests.front();
			m_requests.pop_front();

			if (!Application::instance()->requestToRunService(info)) {
				serviceStartRequestFailed(info);
			}

			m_mutex.unlock();
		}
	}
}

void StartupDispatcher::serviceStartRequestFailed(const ServiceStartupInformation& _serviceInfo) {
	OT_LOG_E("Service startup failed (Name = \"" + _serviceInfo.serviceInformation().name() + "\"; Type = \"" + _serviceInfo.serviceInformation().type() + "\")");

	// Clean up other requests for the same session
	bool erased = false;
	while (erased) {
		erased = false;
		for (std::list<ServiceStartupInformation>::iterator it = m_requests.begin(); it != m_requests.end(); it++) {
			if (it->sessionInformation().id() == _serviceInfo.sessionInformation().id() && 
				it->sessionInformation().sessionServiceURL() == _serviceInfo.sessionInformation().sessionServiceURL())
			{
				m_requests.erase(it);
				erased = true;
				break;
			}
		}
	}

	// Notify session service about the failed startup
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_ServiceStartupFailed);
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_NAME, _serviceInfo.serviceInformation().name());
	ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_TYPE, _serviceInfo.serviceInformation().type());
	ot::rJSON::add(doc, OT_ACTION_PARAM_SESSION_ID, _serviceInfo.sessionInformation().id());

	// Fire message
	ot::msg::sendAsync(Application::instance()->serviceURL(), _serviceInfo.sessionInformation().sessionServiceURL(), ot::EXECUTE, ot::rJSON::toJSON(doc));
}