// Service header
#include "Application.h"
#include "StartupDispatcher.h"
#include "LocalDirectoryService.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCommunication/Msg.h"

// std header
#include <thread>
#include <chrono>

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
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceStartupFailed, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(_serviceInfo.serviceInformation().name(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(_serviceInfo.serviceInformation().type(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_serviceInfo.sessionInformation().id(), doc.GetAllocator()), doc.GetAllocator());
	
	// Send message asynchronously
	ot::msg::sendAsync(Application::instance()->getServiceURL(), _serviceInfo.sessionInformation().sessionServiceURL(), ot::EXECUTE, doc.toJson(), ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit);
}