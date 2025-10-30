// @otlicense
// File: GlobalDirectoryService.cpp
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

// LSS header
#include "SessionService.h"
#include "GlobalDirectoryService.h"

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTCore/ReturnMessage.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"

// std header
#include <thread>
#include <chrono>

GlobalDirectoryService::GlobalDirectoryService() :
	m_serviceInfo(OT_INFO_SERVICE_TYPE_GlobalDirectoryService, OT_INFO_SERVICE_TYPE_GlobalDirectoryService),
	m_connectionStatus(Disconnected), m_healthCheckRunning(false), m_healthCheckThread(nullptr)
{

}

GlobalDirectoryService::~GlobalDirectoryService() {
	this->stopHealthCheck();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Management

bool GlobalDirectoryService::connect(const std::string& _url, bool _waitForConnection) {
		// Clean up potentially running health check thread
		this->stopHealthCheck();

		// Update information
		m_mutex.lock();
		m_serviceInfo.setServiceURL(_url);
		m_connectionStatus = CheckingNewConnection;
		m_mutex.unlock();

		OT_LOG_D("Checking for connection to new GDS at \"" + _url + "\"");

		// Start health check and wait for success
		this->startHealthCheck();

	if (_waitForConnection) {
		// Wait for connection success
		int ct = 0;
		const int maxCt = 6000;
		while (!this->isConnected()) {
			if (ct++ >= maxCt) {
				OT_LOG_E("Failed to connect to Global Session Service at \"" + _url + "\": Timeout reached");
				this->stopHealthCheck();
				return false;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

	}

	return true;
}

bool GlobalDirectoryService::isConnected(void) {
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_connectionStatus == Connected;
}

bool GlobalDirectoryService::requestToStartService(const ot::ServiceInitData& _serviceInformation) {
	// Create request
	ot::JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_StartNewService, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_IniData, ot::JsonObject(_serviceInformation, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	// Send request
	std::string response;
	if (!this->ensureConnection()) {
		OT_LOG_E("Failed to start service \"" + _serviceInformation.getServiceName() + "\". Reason: Failed to establish connection to GDS.");
		return false;
	}
	else if (!this->sendMessage(ot::EXECUTE, requestDoc.toJson(), response)) {
		OT_LOG_E("Failed to start service \"" + _serviceInformation.getServiceName() + "\". Reason: Failed to send http request to GDS.");
		return false;
	}
	else {
		return ot::ReturnMessage::fromJson(response) == ot::ReturnMessage::Ok;
	}
}

bool GlobalDirectoryService::requestToStartServices(const ot::ServiceInitData& _generalData, const std::list<ot::ServiceBase>& _serviceInformation) {
	// Create request
	ot::JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_StartNewServices, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_IniData, ot::JsonObject(_generalData, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	
	ot::JsonArray serviceArr;
	for (const ot::ServiceBase& service : _serviceInformation) {
		ot::JsonObject serviceObj;
		serviceObj.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(service.getServiceName(), requestDoc.GetAllocator()), requestDoc.GetAllocator());
		serviceObj.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(service.getServiceType(), requestDoc.GetAllocator()), requestDoc.GetAllocator());
		serviceObj.AddMember(OT_ACTION_PARAM_SERVICE_ID, service.getServiceID(), requestDoc.GetAllocator());
		serviceArr.PushBack(serviceObj, requestDoc.GetAllocator());
	}
	requestDoc.AddMember(OT_ACTION_PARAM_SESSION_SERVICES, serviceArr, requestDoc.GetAllocator());

	// Send request
	std::string response;
	if (!this->ensureConnection()) {
		OT_LOG_E("Failed to start services. Reason: Failed to establish connection to GDS");
		return false;
	}
	if (!this->sendMessage(ot::EXECUTE, requestDoc.toJson(), response)) {
		OT_LOG_E("Failed to start services. Reason: Failed to send http request to GDS");
		return false;
	}
	else {
		return ot::ReturnMessage::fromJson(response) == ot::ReturnMessage::Ok;
	}
}

bool GlobalDirectoryService::startRelayService(const ot::ServiceInitData& _serviceInformation, std::string& _relayServiceURL, std::string& _websocketURL) {
	// Create request
	ot::JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_StartNewRelayService, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_IniData, ot::JsonObject(_serviceInformation, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	// Send request
	std::string responseStr;
	if (!this->ensureConnection()) {
		OT_LOG_E("Failed to start services. Reason: Failed to establish connection to GDS");
		return false;
	}
	if (!this->sendMessage(ot::EXECUTE, requestDoc.toJson(), responseStr)) {
		OT_LOG_E("Failed to start services. Reason: Failed to send http request to GDS");
		return false;
	}
	else {
		ot::ReturnMessage response = ot::ReturnMessage::fromJson(responseStr);

		if (response.getStatus() != ot::ReturnMessage::Ok) {
			OT_LOG_E("Failed to start relay service. Reason: " + response.getWhat());
			return false;
		}

		// Check reponse
		ot::JsonDocument responseDoc;
		responseDoc.fromJson(response.getWhat());

		_relayServiceURL = ot::json::getString(responseDoc, OT_ACTION_PARAM_SERVICE_URL);
		_websocketURL = ot::json::getString(responseDoc, OT_ACTION_PARAM_WebsocketURL);

		return true;
	}
}

void GlobalDirectoryService::notifySessionShuttingDown(const std::string& _sessionID) {
	ot::JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ShutdownSession, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_sessionID, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_SESSION_SERVICE_URL, ot::JsonString(SessionService::instance().getUrl(), requestDoc.GetAllocator()), requestDoc.GetAllocator());

	std::string responseStr;
	if (!this->ensureConnection()) {
		OT_LOG_E("Failed to start services. Reason: Failed to establish connection to GDS");
	}
	if (!this->sendMessage(ot::EXECUTE, requestDoc.toJson(), responseStr)) {
		OT_LOG_E("Failed to start services. Reason: Failed to send http request to GDS");
	}

	ot::ReturnMessage response = ot::ReturnMessage::fromJson(responseStr);
	if (!response.isOk()) {
		OT_LOG_E("Failed to notify GDS about session shutdown: " + response.getWhat());
	}
}

std::string GlobalDirectoryService::getServiceUrl() {
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_serviceInfo.getServiceURL();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Serialization

void GlobalDirectoryService::addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) {
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_healthCheckRunning) {
		_jsonObject.AddMember("HealthCheckRunning", true, _allocator);
	}
	else {
		_jsonObject.AddMember("HealthCheckRunning", false, _allocator);
	}

	switch (m_connectionStatus) {
	case GlobalDirectoryService::Connected:
		_jsonObject.AddMember("ConnectionStatus", ot::JsonString("Connected", _allocator), _allocator);
		break;

	case GlobalDirectoryService::Disconnected:
		_jsonObject.AddMember("ConnectionStatus", ot::JsonString("Disconnected", _allocator), _allocator);
		break;

	case GlobalDirectoryService::CheckingNewConnection:
		_jsonObject.AddMember("ConnectionStatus", ot::JsonString("CheckingNewConnection", _allocator), _allocator);
		break;

	default:
		OT_LOG_EAS("Unknown connection status (" + std::to_string(static_cast<int>(m_connectionStatus)) + ")");
		break;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private

void GlobalDirectoryService::startHealthCheck(void) {
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_healthCheckRunning) {
		OT_LOG_EA("Health check already running");
		return;
	}

	m_healthCheckRunning = true;

	OT_LOG_D("Starting health check");
	m_healthCheckThread = new std::thread(&GlobalDirectoryService::healthCheck, this);
}

void GlobalDirectoryService::stopHealthCheck() {
	m_mutex.lock();

	if (m_healthCheckThread) {
		m_healthCheckRunning = false;

		m_mutex.unlock();

		if (m_healthCheckThread->joinable()) {
			m_healthCheckThread->join();
		}

		m_mutex.lock();

		delete m_healthCheckThread;
		m_healthCheckThread = nullptr;

		m_mutex.unlock();
	}
	else {
		m_mutex.unlock();
	}
}

void GlobalDirectoryService::healthCheck(void) {
	ot::JsonDocument pingDoc;
	pingDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Ping, pingDoc.GetAllocator()), pingDoc.GetAllocator());
	std::string pingMessage = pingDoc.toJson();

	int timeout = 60;
	std::string lssUrl = SessionService::instance().getUrl();

	OT_LOG_D("Starting health check worker...");

	while (m_healthCheckRunning) {
		// Check connection
		std::string pingResponse;

		if (!this->sendMessage(ot::EXECUTE, pingMessage, pingResponse)) {
			// Handle disconnected
			std::lock_guard<std::mutex> lock(m_mutex);
			m_connectionStatus = Disconnected;
		}
		else if (pingResponse == OT_ACTION_CMD_Ping) {
			std::lock_guard<std::mutex> lock(m_mutex);

			if (m_connectionStatus != Connected) {
				OT_LOG_D("Successfully checked connection to GDS");
				m_connectionStatus = Connected;
			}
		}
		else {
			OT_LOG_E("Invalid ping response received from GDS: \"" + pingResponse + "\".");

			// Handle disconnected
			std::lock_guard<std::mutex> lock(m_mutex);
			m_connectionStatus = Disconnected;
		}

		for (int ct = 0; ct < 60 && m_healthCheckRunning; ct++) {
			std::this_thread::sleep_for(std::chrono::seconds(1));

			if (!this->isConnected()) {
				break;
			}
		}
	}

	OT_LOG_D("Health check worker stopped...");
}

bool GlobalDirectoryService::ensureConnection() {
	int ct = 0;
	const int checkTimeout = 10;
	const int checkAttempts = 60 * (1000 / checkTimeout);

	while (!this->isConnected()) {
		if (++ct >= checkAttempts) {
			return false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(checkTimeout));
	}

	return true;
}

bool GlobalDirectoryService::sendMessage(ot::MessageType _messageType, const std::string& _message, std::string& _response) {
	m_mutex.lock();
	std::string receiverUrl = m_serviceInfo.getServiceURL();
	m_mutex.unlock();

	if (!ot::msg::send("", receiverUrl, _messageType, _message, _response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		m_mutex.lock();
		m_connectionStatus = Disconnected;
		m_mutex.unlock();
		return false;
	}
	else {
		return true;
	}
}
