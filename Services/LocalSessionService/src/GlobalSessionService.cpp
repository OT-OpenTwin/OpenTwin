// @otlicense

// LSS header
#include "Session.h"
#include "SessionService.h"
#include "GlobalSessionService.h"

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/ReturnMessage.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"

// std header
#include <thread>
#include <chrono>

GlobalSessionService::GlobalSessionService()
	: m_serviceInfo(OT_INFO_SERVICE_TYPE_GlobalSessionService, OT_INFO_SERVICE_TYPE_GlobalSessionService),
	m_connectionStatus(Disconnected), m_healthCheckRunning(false), m_workerThread(nullptr)
{

}

GlobalSessionService::~GlobalSessionService() {

}

bool GlobalSessionService::connect(const std::string& _url) {
	{
		m_mutex.lock();

		// Ensure connection status is disconnected
		if (m_connectionStatus != Disconnected) {
			m_mutex.unlock();

			OT_LOG_E("GSS state is not Disconnected. Ignoring request for \"" + _url + "\"");
			return false;
		}
		else {
			m_mutex.unlock();
		}

		// Clean up potentially running health check thread
		this->stopHealthCheck();
		
		// Update information
		m_serviceInfo.setServiceURL(_url);
		m_connectionStatus = CheckingNewConnection;

		OT_LOG_D("Checking for connection to new GDS at \"" + _url + "\"");

		this->startHealthCheck();
	}
	
	// Wait for health check to finish
	int ct = 0;
	const int maxCt = 60;
	while (!this->isConnected()) {
		if (ct++ >= maxCt) {
			OT_LOG_E("Failed to connect to Global Session Service at \"" + _url + "\" after " + std::to_string(maxCt) + " attempts");
			this->stopHealthCheck();
			return false;
		}

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	return true;
}

std::string GlobalSessionService::getServiceUrl() {
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_serviceInfo.getServiceURL();
}

bool GlobalSessionService::isConnected() {
	std::lock_guard<std::mutex> lock(m_mutex);
	return (m_connectionStatus == Connected);
}

bool GlobalSessionService::confirmSession(const std::string& _sessionID, const std::string& _userName) {
	SessionService& lss = SessionService::instance();

	ot::JsonDocument confirmDoc;
	confirmDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ConfirmSession, confirmDoc.GetAllocator()), confirmDoc.GetAllocator());
	confirmDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_sessionID, confirmDoc.GetAllocator()), confirmDoc.GetAllocator());
	confirmDoc.AddMember(OT_ACTION_PARAM_USER_NAME, ot::JsonString(_userName, confirmDoc.GetAllocator()), confirmDoc.GetAllocator());
	confirmDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_registrationResult.getServiceID(), confirmDoc.GetAllocator());
	
	// Send ping
	std::string responseStr;
	if (!this->sendMessage(ot::EXECUTE, confirmDoc.toJson(), responseStr)) {
		return false;
	}
	
	// Read result
	ot::ReturnMessage msg = ot::ReturnMessage::fromJson(responseStr);
	if (msg == ot::ReturnMessage::Ok) {
		return true;
	}
	else {
		OT_LOG_E("Failed to confirm session \"" + _sessionID + "\". Message: \"" + msg.getWhat() + "\"");
		return false;
	}
}

bool GlobalSessionService::notifySessionShutdownCompleted(const std::string& _sessionID) {
	SessionService& lss = SessionService::instance();

	ot::JsonDocument confirmDoc;
	confirmDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ShutdownSessionCompleted, confirmDoc.GetAllocator()), confirmDoc.GetAllocator());
	confirmDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_sessionID, confirmDoc.GetAllocator()), confirmDoc.GetAllocator());
	confirmDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_registrationResult.getServiceID(), confirmDoc.GetAllocator());

	// Send ping
	std::string responseStr;
	if (!this->sendMessage(ot::EXECUTE, confirmDoc.toJson(), responseStr)) {
		return false;
	}

	// Read result
	ot::ReturnMessage msg = ot::ReturnMessage::fromJson(responseStr);
	if (msg == ot::ReturnMessage::Ok) {
		return true;
	}
	else {
		OT_LOG_E("Failed to notify session closed \"" + _sessionID + "\". Message: \"" + msg.getWhat() + "\"");
		return false;
	}
}

void GlobalSessionService::setGDSUrl(const std::string& _gdsUrl) {
	std::lock_guard<std::mutex> lock(m_mutex);
	m_registrationResult.setGdsURL(_gdsUrl);
}

GSSRegistrationInfo GlobalSessionService::getRegistrationResult() {
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_registrationResult;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Serialization

void GlobalSessionService::addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) {
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_healthCheckRunning) {
		_jsonObject.AddMember("HealthCheckRunning", true, _allocator);
	}
	else {
		_jsonObject.AddMember("HealthCheckRunning", false, _allocator);
	}

	switch (m_connectionStatus) {
	case GlobalSessionService::Connected:
		_jsonObject.AddMember("ConnectionStatus", ot::JsonString("Connected", _allocator), _allocator);
		break;

	case GlobalSessionService::Disconnected:
		_jsonObject.AddMember("ConnectionStatus", ot::JsonString("Disconnected", _allocator), _allocator);
		break;

	case GlobalSessionService::CheckingNewConnection:
		_jsonObject.AddMember("ConnectionStatus", ot::JsonString("CheckingNewConnection", _allocator), _allocator);
		break;

	default:
		OT_LOG_EAS("Unknown connection status (" + std::to_string(static_cast<int>(m_connectionStatus)) + ")");
		break;
	}

	ot::JsonObject registrationInfo;
	m_registrationResult.addToJsonObject(registrationInfo, _allocator);
	_jsonObject.AddMember("RegistrationResult", registrationInfo, _allocator);
}

// #################################################################################################################################################

// Private functions

bool GlobalSessionService::ensureConnection() {
	int attemt = 0;
	const int attemptSleepTime = 10;
	const int attemtLimit = 60 * (1000 / attemptSleepTime);
	while (!this->isConnected()) {
		if (++attemt >= attemtLimit) {
			OT_LOG_E("Failed to send message to LSS: Failed to establish connection");
			m_mutex.lock();
			m_connectionStatus = Disconnected;
			m_mutex.unlock();
			return false;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(attemptSleepTime));
	}

	return true;
}

bool GlobalSessionService::sendMessage(ot::MessageType _messageType, const std::string& _message, std::string& _response) {
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

void GlobalSessionService::startHealthCheck() {
	std::lock_guard<std::mutex> lock(m_mutex);

	if (m_healthCheckRunning) {
		OT_LOG_EA("Health check already running");
		return;
	}

	OTAssert(m_workerThread == nullptr, "Health check thread already exists");

	m_healthCheckRunning = true;

	OT_LOG_D("Starting health check");
	m_workerThread = new std::thread(&GlobalSessionService::healthCheck, this);
}

void GlobalSessionService::stopHealthCheck() {
	m_mutex.lock();

	if (m_workerThread) {
		m_healthCheckRunning = false;

		m_mutex.unlock();

		if (m_workerThread->joinable()) {
			m_workerThread->join();
		}

		m_mutex.lock();

		delete m_workerThread;
		m_workerThread = nullptr;

		m_mutex.unlock();
	}
	else {
		m_mutex.unlock();
	}
}

void GlobalSessionService::healthCheck() {
	int ct;
	ot::JsonDocument pingDoc;
	pingDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_PING, pingDoc.GetAllocator()), pingDoc.GetAllocator());
	std::string pingMessage = pingDoc.toJson();

	std::string lssUrl = SessionService::instance().getUrl();

	while (m_healthCheckRunning) {
		std::string pingResponse;

		// Send ping
		if (!this->sendMessage(ot::EXECUTE, pingMessage, pingResponse)) {
			m_mutex.lock();
			m_connectionStatus = Disconnected;
			m_mutex.unlock();

			continue;
		}

		// Check response
		if (pingResponse != OT_ACTION_PING) {
			OT_LOG_W("Received invalid ping response from global session service");
			m_mutex.lock();
			m_connectionStatus = Disconnected;
			m_mutex.unlock();
		}
		else if (!this->isConnected()) {
			// Register at the session service
			ot::JsonDocument registerDoc;
			registerDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_RegisterNewSessionService, registerDoc.GetAllocator()), registerDoc.GetAllocator());
			registerDoc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(lssUrl, registerDoc.GetAllocator()), registerDoc.GetAllocator());
			registerDoc.AddMember(OT_ACTION_PARAM_Sessions, ot::JsonArray(SessionService::instance().getSessionIDs(), registerDoc.GetAllocator()), registerDoc.GetAllocator());

			ot::JsonArray iniListArr;
			registerDoc.AddMember(OT_ACTION_PARAM_IniList, iniListArr, registerDoc.GetAllocator());

			std::string registerResponse;

			// Send registration
			if (!this->sendMessage(ot::EXECUTE, registerDoc.toJson(), registerResponse)) {
				OT_LOG_E("Failed to send register message to global session service");
				continue;
			}

			ot::ReturnMessage response = ot::ReturnMessage::fromJson(registerResponse);

			if (response != ot::ReturnMessage::Ok) {
				OT_LOG_E("Failed to register at GSS: " + response.getWhat());
				continue;
			}

			// Get new id and database url
			ot::JsonDocument registrationResponseDoc;
			registrationResponseDoc.fromJson(response.getWhat());

			GSSRegistrationInfo info;
			info.setServiceID(ot::json::getUInt(registrationResponseDoc, OT_ACTION_PARAM_SERVICE_ID));
			info.setDataBaseURL(ot::json::getString(registrationResponseDoc, OT_ACTION_PARAM_DATABASE_URL));
			info.setAuthURL(ot::json::getString(registrationResponseDoc, OT_ACTION_PARAM_SERVICE_AUTHURL));

			if (registrationResponseDoc.HasMember(OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL)) {
				info.setGdsURL(ot::json::getString(registrationResponseDoc, OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL));
			}
			if (registrationResponseDoc.HasMember(OT_ACTION_PARAM_LIBRARYMANAGEMENT_SERVICE_URL)) {
				info.setLMSURL(ot::json::getString(registrationResponseDoc, OT_ACTION_PARAM_LIBRARYMANAGEMENT_SERVICE_URL));
			}

			// Get global logger info
			info.setLoggingURL(ot::json::getString(registrationResponseDoc, OT_ACTION_PARAM_GlobalLoggerUrl));
			ot::ConstJsonArray logFlags = ot::json::getArray(registrationResponseDoc, OT_ACTION_PARAM_GlobalLogFlags);
			info.setLogFlags(ot::logFlagsFromJsonArray(logFlags));

			m_mutex.lock();

			m_registrationResult = std::move(info);

			// Set connection status to connected
			m_connectionStatus = Connected;

			m_mutex.unlock();
		}

		ct = 0;
		while (ct++ < 6000 && m_healthCheckRunning) { // Wait for 1 min
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			if (!this->isConnected()) {
				break;
			}
		}
	}
}
