//! @file GlobalDirectoryService.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2025
// ###########################################################################################################################################################################################################################################################################################################################

// LSS header
#include "SessionService.h"
#include "GlobalDirectoryService.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"

// std header
#include <thread>
#include <chrono>

GlobalDirectoryService::GlobalDirectoryService() :
	ot::ServiceBase(OT_INFO_SERVICE_TYPE_GlobalDirectoryService, OT_INFO_SERVICE_TYPE_GlobalDirectoryService),
	m_connectionStatus(Disconnected), m_isShuttingDown(false), m_healthCheckThread(nullptr)
{

}

GlobalDirectoryService::~GlobalDirectoryService() {
	if (m_healthCheckThread) {
		m_isShuttingDown = true;
		m_healthCheckThread->join();
		delete m_healthCheckThread;
		m_healthCheckThread = nullptr;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Management

void GlobalDirectoryService::connect(const std::string& _url) {
	std::lock_guard<std::mutex> lock(m_mutex);

	// Ensure connection status is disconnected
	if (m_connectionStatus != Disconnected) {
		OT_LOG_E("GDS state is not Disconnected. Ignoring request for \"" + _url + "\"");
		return;
	}

	// Clean up potentially running health check thread
	if (m_healthCheckThread) {
		m_isShuttingDown = true;
		m_healthCheckThread->join();
		delete m_healthCheckThread;
		m_healthCheckThread = nullptr;
	}

	// Update information
	this->setServiceURL(_url);

	m_isShuttingDown = false;
	m_connectionStatus = CheckingNewConnection;
	
	OT_LOG_D("Checking for connection to new GDS at \"" + _url + "\"");

	m_healthCheckThread = new std::thread(&GlobalDirectoryService::healthCheck, this);
}

bool GlobalDirectoryService::isConnected(void) {
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_connectionStatus == Connected;
}

bool GlobalDirectoryService::requestToStartService(const ot::ServiceBase& _serviceInformation, const std::string& _sessionID) {
	std::lock_guard<std::mutex> lock(m_mutex);

	std::string lssUrl = SessionService::instance().getUrl();

	// Create request
	ot::JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_StartNewService, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(_serviceInformation.getServiceName(), requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(_serviceInformation.getServiceType(), requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, _serviceInformation.getServiceID(), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_sessionID, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_SESSION_SERVICE_URL, ot::JsonString(lssUrl, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	// Send request
	std::string response;
	if (!ot::msg::send(lssUrl, m_serviceURL, ot::EXECUTE, requestDoc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_E("Failed to start services. Reason: Failed to send http request to GDS (URL = \"" + m_serviceURL + "\")");
	}
	else if (response != OT_ACTION_RETURN_VALUE_OK) {
		OT_LOG_E("Failed to start services. Reason: Invalid response: \"" + response + "\". Expected: \"" OT_ACTION_RETURN_VALUE_OK "\"");
	}
	else {
		return true;
	}

	return false;
}

bool GlobalDirectoryService::requestToStartServices(const std::list<ot::ServiceBase>& _serviceInformation, const std::string& _sessionID) {
	std::lock_guard<std::mutex> lock(m_mutex);

	std::string lssUrl = SessionService::instance().getUrl();

	// Create request
	ot::JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_StartNewServices, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_sessionID, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_SESSION_SERVICE_URL, ot::JsonString(lssUrl, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	
	ot::JsonArray serviceArr;
	for (const ServiceBase& service : _serviceInformation) {
		ot::JsonObject serviceObj;
		serviceObj.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(service.getServiceName(), requestDoc.GetAllocator()), requestDoc.GetAllocator());
		serviceObj.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(service.getServiceType(), requestDoc.GetAllocator()), requestDoc.GetAllocator());
		serviceObj.AddMember(OT_ACTION_PARAM_SERVICE_ID, service.getServiceID(), requestDoc.GetAllocator());
		serviceArr.PushBack(serviceObj, requestDoc.GetAllocator());
	}
	requestDoc.AddMember(OT_ACTION_PARAM_SESSION_SERVICES, serviceArr, requestDoc.GetAllocator());

	// Send request
	std::string response;
	if (!ot::msg::send(lssUrl, m_serviceURL, ot::EXECUTE, requestDoc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_E("Failed to start services. Reason: Failed to send http request to GDS (URL = \"" + m_serviceURL + "\")");
	}
	else if (response != OT_ACTION_RETURN_VALUE_OK) {
		OT_LOG_E("Failed to start services. Reason: Invalid response: \"" + response + "\". Expected: \"" OT_ACTION_RETURN_VALUE_OK "\"");
	}
	else {
		return true;
	}

	return false;
}

bool GlobalDirectoryService::startRelayService(ot::serviceID_t _serviceID, const std::string& _sessionID, std::string& _websocketURL, std::string& _relayServiceURL) {
	std::lock_guard<std::mutex> lock(m_mutex);

	std::string lssUrl = SessionService::instance().getUrl();

	// Create request
	ot::JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_StartNewRelayService, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, _serviceID, requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_sessionID, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_SESSION_SERVICE_URL, ot::JsonString(lssUrl, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	
	// Send request
	std::string response;
	if (!ot::msg::send(lssUrl, m_serviceURL, ot::EXECUTE, requestDoc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_E("Failed to start relay service. Reason: Failed to send http request to GDS (URL = \"" + m_serviceURL + "\")");
		return false;
	}
	if (response.find(OT_ACTION_RETURN_INDICATOR_Error) != std::string::npos) {
		OT_LOG_E("Failed to start relay service. Reason: Error response from GDS (URL = \"" + m_serviceURL + "\"): " + response);
		return false;
	}
	if (response.find(OT_ACTION_RETURN_INDICATOR_Warning) != std::string::npos) {
		OT_LOG_E("Failed to start relay service. Reason: Warning response from GDS (URL = \"" + m_serviceURL + "\"): " + response);
		return false;
	}
	if (response == OT_ACTION_RETURN_VALUE_FAILED) {
		OT_LOG_E("Failed to start relay service. Reason: GDS reported start failed (URL = \"" + m_serviceURL + "\")");
		return false;
	}

	// Check reponse
	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);

	_relayServiceURL = ot::json::getString(responseDoc, OT_ACTION_PARAM_SERVICE_URL);
	_websocketURL = ot::json::getString(responseDoc, OT_ACTION_PARAM_WebsocketURL);

	return true;
}

void GlobalDirectoryService::notifySessionClosed(const std::string& _sessionID) {
	std::lock_guard<std::mutex> lock(m_mutex);

	ot::JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ShutdownSessionCompleted, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_sessionID, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_SESSION_SERVICE_URL, ot::JsonString(SessionService::instance().getUrl(), requestDoc.GetAllocator()), requestDoc.GetAllocator());

	std::string response;
	if (!ot::msg::send(SessionService::instance().getUrl(), m_serviceURL, ot::EXECUTE, requestDoc.toJson(), response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_E("Failed to send session shutdown notification to GDS at \"" + m_serviceURL + "\"");
	}
	if (response != OT_ACTION_RETURN_VALUE_OK) {
		OT_LOG_E("Unexpected GDS response \"" + m_serviceURL + "\": \"" + response + "\"");
	}
}

void GlobalDirectoryService::healthCheck(void) {
	ot::JsonDocument pingDoc;
	pingDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Ping, pingDoc.GetAllocator()), pingDoc.GetAllocator());
	std::string pingMessage = pingDoc.toJson();

	int timeout = 60;
	std::string lssUrl = SessionService::instance().getUrl();

	OT_LOG_D("Starting health check worker...");

	while (!m_isShuttingDown) {
		
		// Check connection
		std::string response;

		if (ot::msg::send(lssUrl, m_serviceURL, ot::EXECUTE, pingMessage, response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
			if (response == OT_ACTION_CMD_Ping) {
				m_mutex.lock();

				if (m_connectionStatus == CheckingNewConnection) {
					OT_LOG_D("Successfully checked new connection to GDS at \"" + m_serviceURL + "\"");
					m_connectionStatus = Connected;
				}
				else {
					m_connectionStatus = Connected;
				}
				
				m_mutex.unlock();

				for (int ct = 0; ct < 60 && !m_isShuttingDown; ct++) {
					using namespace std::chrono_literals;
					std::this_thread::sleep_for(1s);
				}
			}
			else {
				OT_LOG_E("Invalid ping response received from GDS at \"" + m_serviceURL + "\"");
				
				// Handle disconnected
				m_mutex.lock();
				m_connectionStatus = Disconnected;
				m_mutex.unlock();
				break;
			}
		}
		else {
			OT_LOG_E("Failed to send ping to GDS at \"" + m_serviceURL + "\"");

			// Handle disconnected
			m_mutex.lock();
			m_connectionStatus = Disconnected;
			m_mutex.unlock();
			break;
		}
	}

	OT_LOG_D("Health check worker stopped...");
}