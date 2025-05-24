//! @file StartupDispatcher.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

// GDS header
#include "Application.h"
#include "StartupDispatcher.h"
#include "LocalDirectoryService.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCommunication/Msg.h"

// std header
#include <thread>
#include <chrono>

StartupDispatcher::StartupDispatcher(void) : 
	m_isStopping(false), m_workerThread(nullptr)
{}

StartupDispatcher::~StartupDispatcher() {
	this->stop();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Request management

void StartupDispatcher::addRequest(ServiceInformation&& _info) {
	std::lock_guard<std::mutex> lock(m_mutex);

	m_requestedServices.push_back(std::move(_info));
	
	this->run();
}

void StartupDispatcher::addRequest(std::list<ServiceInformation>&& _info) {
	std::lock_guard<std::mutex> lock(m_mutex);

	m_requestedServices.splice(m_requestedServices.end(), std::move(_info));
	
	this->run();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private methods

void StartupDispatcher::run(void) {
	if (m_workerThread) {
		return;
	}

	m_isStopping = false;
	m_workerThread = new std::thread(&StartupDispatcher::workerFunction, this);
}

void StartupDispatcher::stop(void) {
	if (m_workerThread == nullptr) {
		return;
	}

	m_isStopping = true;
	
	m_workerThread->join();
	
	delete m_workerThread;
	m_workerThread = nullptr;
}

void StartupDispatcher::workerFunction(void) {
	using namespace std::chrono_literals;

	while (!m_isStopping) {
		// Check if there are any queued requests
		while (!m_isStopping) {
			{
				std::lock_guard<std::mutex> lock(m_mutex);

				if (!m_requestedServices.empty()) {
					break;
				}
			}

			std::this_thread::sleep_for(100ms);
		}

		// Dispatch queued requests
		while (!m_isStopping) {
			std::lock_guard<std::mutex> lock(m_mutex);

			if (m_requestedServices.empty()) {
				break;
			}
			
			// Grab first item
			ServiceInformation info(std::move(m_requestedServices.front()));
			m_requestedServices.pop_front();

			if (!Application::instance().requestToRunService(info)) {
				this->serviceStartRequestFailed(info);
			}
		}
	}
}

void StartupDispatcher::serviceStartRequestFailed(const ServiceInformation& _serviceInfo) {
	OT_LOG_E("Service startup failed { \"Name\": \"" + _serviceInfo.getName() + "\", \"Type\": \"" + _serviceInfo.getType() + "\" }");

	// Clean up other requests for the same session
	bool erased = false;
	while (erased) {
		erased = false;
		for (auto it = m_requestedServices.begin(); it != m_requestedServices.end(); it++) {
			if (it->getSessionInformation() == _serviceInfo.getSessionInformation()) {
				m_requestedServices.erase(it);
				erased = true;
				break;
			}
		}
	}

	// Notify session service about the failed startup
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceStartupFailed, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(_serviceInfo.getName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(_serviceInfo.getType(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_serviceInfo.getSessionId(), doc.GetAllocator()), doc.GetAllocator());
	
	// Send message asynchronously
	ot::msg::sendAsync(Application::instance().getServiceURL(), _serviceInfo.getSessionServiceURL(), ot::EXECUTE, doc.toJson(), ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit);
}