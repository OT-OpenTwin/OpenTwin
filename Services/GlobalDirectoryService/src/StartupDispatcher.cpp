// @otlicense
// File: StartupDispatcher.cpp
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

// GDS header
#include "Application.h"
#include "StartupDispatcher.h"
#include "LocalDirectoryService.h"

// OpenTwin header
#include "OTCore/LogDispatcher.h"
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

void StartupDispatcher::addRequest(ot::ServiceInitData&& _info) {
	std::lock_guard<std::mutex> lock(m_mutex);

	m_requestedServices.push_back(std::move(_info));
	
	this->run();
}

void StartupDispatcher::addRequest(std::list<ot::ServiceInitData>&& _info) {
	std::lock_guard<std::mutex> lock(m_mutex);

	m_requestedServices.splice(m_requestedServices.end(), std::move(_info));
	
	this->run();
}

void StartupDispatcher::sessionClosing(const std::string& _sessionID) {
	std::lock_guard<std::mutex> lock(m_mutex);
	this->removeRequestedServices(_sessionID);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Serialization

void StartupDispatcher::getDebugInformation(ot::GDSDebugInfo& _debugInfo) {
	std::lock_guard<std::mutex> lock(m_mutex);

	_debugInfo.setStartupWorkerRunning(m_workerThread != nullptr);
	_debugInfo.setStartupWorkerStopping(m_isStopping);

	for (const ot::ServiceInitData& info : m_requestedServices) {
		_debugInfo.addRequestedService(_debugInfo.infoFromInitData(info));
	}
}

void StartupDispatcher::addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) {
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_isStopping) {
		_jsonObject.AddMember("IsStopping", true, _allocator);
	}
	else {
		_jsonObject.AddMember("IsStopping", false, _allocator);
	}

	ot::JsonArray requestedArr;
	for (const ot::ServiceInitData& info : m_requestedServices) {
		ot::JsonObject obj;
		info.addToJsonObject(obj, _allocator);
		requestedArr.PushBack(obj, _allocator);
	}
	_jsonObject.AddMember("RequestedServices", requestedArr, _allocator);
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
	while (!m_isStopping) {
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			while (!m_requestedServices.empty()) {
				// Grab first item
				ot::ServiceInitData info(std::move(m_requestedServices.front()));
				m_requestedServices.pop_front();

				if (!Application::instance().requestToRunService(info)) {
					this->serviceStartRequestFailed(info);
				}
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void StartupDispatcher::removeRequestedServices(const std::string& _sessionID) {
	for (auto it = m_requestedServices.begin(); it != m_requestedServices.end(); ) {
		if (it->getSessionID() == _sessionID) {
			it = m_requestedServices.erase(it);
		}
		else {
			it++;
		}
	}
}

void StartupDispatcher::serviceStartRequestFailed(const ot::ServiceInitData& _serviceInfo) {
	OT_LOG_E(
		"Service startup failed { \"ID\": " + std::to_string(_serviceInfo.getServiceID()) + ", "
		"\"Name\": \"" + _serviceInfo.getServiceName() + "\", "
		"\"Type\": \"" + _serviceInfo.getServiceType() + "\", "
		"\"SessionID\": \"" + _serviceInfo.getSessionID() + "\", "
		"\"LssUrl\": \"" + _serviceInfo.getSessionServiceURL() + "\""
		" }"
	);

	// Clean up other requests for the same session
	this->removeRequestedServices(_serviceInfo.getSessionID());

	// Notify session service about the failed startup
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ServiceStartupFailed, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_ID, _serviceInfo.getServiceID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_serviceInfo.getSessionID(), doc.GetAllocator()), doc.GetAllocator());
	
	// Send message
	std::string response;
	ot::msg::send(Application::instance().getServiceURL(), _serviceInfo.getSessionServiceURL(), ot::EXECUTE, doc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit);
}