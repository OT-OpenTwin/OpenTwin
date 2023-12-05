#include "GlobalDirectoryService.h"
#include "SessionService.h"

#include "OTCore/Logger.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"

#include <thread>

GlobalDirectoryService::GlobalDirectoryService(SessionService * _sessionService)
	: ot::ServiceBase(OT_INFO_SERVICE_TYPE_GlobalDirectoryService, OT_INFO_SERVICE_TYPE_GlobalDirectoryService),
	m_connectionStatus(Disconnected), m_sessionService(_sessionService), m_isShuttingDown(false)
{

}

GlobalDirectoryService::~GlobalDirectoryService() {

}

void GlobalDirectoryService::connect(const std::string& _url) {
	setServiceURL(_url);
	m_isShuttingDown = false;
	m_connectionStatus = CheckingNewConnection;
	
	OT_LOG_D("Checking for connection to new GDS at: " + _url);

	std::thread t(&GlobalDirectoryService::healthCheck, this);
	t.detach();
}

bool GlobalDirectoryService::isConnected(void) const {
	return m_connectionStatus == Connected;
}

bool GlobalDirectoryService::requestToStartService(const ot::ServiceBase& _serviceInformation, const std::string& _sessionID) {
	// Create request
	ot::JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_StartNewService, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(_serviceInformation.serviceName(), requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(_serviceInformation.serviceType(), requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_sessionID, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_SESSION_SERVICE_URL, ot::JsonString(m_sessionService->url(), requestDoc.GetAllocator()), requestDoc.GetAllocator());

	// Send request
	std::string response;
	if (!ot::msg::send(m_sessionService->url(), m_serviceURL, ot::EXECUTE, requestDoc.toJson(), response)) {
		OT_LOG_E("Failed to start services. Reason: Failed to send http request to GDS (URL = \"" + m_serviceURL + "\")");
		return false;
	}
	if (response.find(OT_ACTION_RETURN_INDICATOR_Error) != std::string::npos) {
		OT_LOG_E("Failed to start services. Reason: Error response: " + response);
		return false;
	}
	if (response.find(OT_ACTION_RETURN_INDICATOR_Warning) != std::string::npos) {
		OT_LOG_E("Failed to start services. Reason: Warning response: " + response);
		return false;
	}
	if (response != OT_ACTION_RETURN_VALUE_OK) {
		OT_LOG_E("Failed to start services. Reason: Invalid response: \"" + response + "\". Expected: \"" OT_ACTION_RETURN_VALUE_OK "\"");
		return false;
	}

	return true;
}

bool GlobalDirectoryService::requestToStartServices(const std::list<ot::ServiceBase>& _serviceInformation, const std::string& _sessionID) {
	// Create request
	ot::JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_StartNewServices, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_sessionID, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_SESSION_SERVICE_URL, ot::JsonString(m_sessionService->url(), requestDoc.GetAllocator()), requestDoc.GetAllocator());
	
	ot::JsonArray serviceArr;
	for (auto s : _serviceInformation) {
		ot::JsonObject serviceObj;
		serviceObj.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(s.serviceName(), requestDoc.GetAllocator()), requestDoc.GetAllocator());
		serviceObj.AddMember(OT_ACTION_PARAM_SERVICE_TYPE, ot::JsonString(s.serviceType(), requestDoc.GetAllocator()), requestDoc.GetAllocator());
		serviceArr.PushBack(serviceObj, requestDoc.GetAllocator());
	}
	requestDoc.AddMember(OT_ACTION_PARAM_SESSION_SERVICES, serviceArr, requestDoc.GetAllocator());

	// Send request
	std::string response;
	if (!ot::msg::send(m_sessionService->url(), m_serviceURL, ot::EXECUTE, requestDoc.toJson(), response)) {
		OT_LOG_E("Failed to start services. Reason: Failed to send http request to GDS (URL = \"" + m_serviceURL + "\")");
		return false;
	}
	if (response.find(OT_ACTION_RETURN_INDICATOR_Error) != std::string::npos) {
		OT_LOG_E("Failed to start services. Reason: Error response: " + response);
		return false;
	}
	if (response.find(OT_ACTION_RETURN_INDICATOR_Warning) != std::string::npos) {
		OT_LOG_E("Failed to start services. Reason: Warning response: " + response);
		return false;
	}
	if (response != OT_ACTION_RETURN_VALUE_OK) {
		OT_LOG_E("Failed to start services. Reason: Invalid response: \"" + response + "\". Expected: \"" OT_ACTION_RETURN_VALUE_OK "\"");
		return false;
	}

	return true;
}

bool GlobalDirectoryService::requestToStartRelayService(const std::string& _sessionID, std::string& _websocketURL, std::string& _relayServiceURL) {
	// Create request
	ot::JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_StartNewRelayService, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_sessionID, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_SESSION_SERVICE_URL, ot::JsonString(m_sessionService->url(), requestDoc.GetAllocator()), requestDoc.GetAllocator());
	
	// Send request
	std::string response;
	if (!ot::msg::send(m_sessionService->url(), m_serviceURL, ot::EXECUTE, requestDoc.toJson(), response)) return false;
	if (response.find(OT_ACTION_RETURN_INDICATOR_Error) != std::string::npos) return false;
	if (response.find(OT_ACTION_RETURN_INDICATOR_Warning) != std::string::npos) return false;
	if (response == OT_ACTION_RETURN_VALUE_FAILED) return false;

	// Check reponse
	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);
	if (!responseDoc.IsObject()) return false;

	if (!responseDoc.HasMember(OT_ACTION_PARAM_SERVICE_URL)) return false;
	if (!responseDoc.HasMember(OT_ACTION_PARAM_WebsocketURL)) return false;
	if (!responseDoc[OT_ACTION_PARAM_SERVICE_URL].IsString()) return false;
	if (!responseDoc[OT_ACTION_PARAM_WebsocketURL].IsString()) return false;
	_relayServiceURL = responseDoc[OT_ACTION_PARAM_SERVICE_URL].GetString();
	_websocketURL = responseDoc[OT_ACTION_PARAM_WebsocketURL].GetString();

	return true;
}

void GlobalDirectoryService::notifySessionClosed(const std::string& _sessionID) {
	ot::JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ShutdownSessionCompleted, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_sessionID, requestDoc.GetAllocator()), requestDoc.GetAllocator());
	requestDoc.AddMember(OT_ACTION_PARAM_SESSION_SERVICE_URL, ot::JsonString(SessionService::instance()->url(), requestDoc.GetAllocator()), requestDoc.GetAllocator());

	std::string response;
	if (!ot::msg::send(SessionService::instance()->url(), m_serviceURL, ot::EXECUTE, requestDoc.toJson(), response)) {
		OT_LOG_E("Failed to send session shutdown notification to GDS at " + m_serviceURL);
	}
	if (response != OT_ACTION_RETURN_VALUE_OK) {
		OT_LOG_E("Failed to send session shutdown notification to GDS at " + m_serviceURL);
	}
}

void GlobalDirectoryService::healthCheck(void) {
	ot::JsonDocument pingDoc;
	pingDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Ping, pingDoc.GetAllocator()), pingDoc.GetAllocator());
	std::string pingMessage = pingDoc.toJson();

	int timeout = 60;

	OT_LOG_D("Starting health check worker");

	while (!m_isShuttingDown) {
		// Check connection
		std::string response;
		// todo: scr: code rm line !current 1
		OT_LOG_D("!!!! ######################################################################################");
		if (ot::msg::send(m_sessionService->url(), m_serviceURL, ot::EXECUTE, pingMessage, response)) {
			if (response == OT_ACTION_CMD_Ping) {
				if (m_connectionStatus == CheckingNewConnection) {
					m_connectionStatus = Connected;
					OT_LOG_D("New connection to GDS successfully checked");
				}
				else {
					m_connectionStatus = Connected;
				}
				
				int ct = 0;
				while (ct++ < 60 && !m_isShuttingDown) {
					using namespace std::chrono_literals;
					std::this_thread::sleep_for(1s);
				}
			}
			else {
				OT_LOG_E("Invalid ping response received from GDS at: " + m_serviceURL);
				// Handle disconnected
				m_connectionStatus = Disconnected;
				break;
			}
		}
		else {
			OT_LOG_E("Failed to send ping to GDS at: " + m_serviceURL);
			// Handle disconnected
			m_connectionStatus = Disconnected;
			break;
		}
	}

	OT_LOG_W("Health check worker stopped...");
}