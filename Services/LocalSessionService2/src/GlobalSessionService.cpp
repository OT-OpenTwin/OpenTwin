//! @file GlobalSessionService.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2025
// ###########################################################################################################################################################################################################################################################################################################################

// LSS header
#include "Session.h"
#include "SessionService.h"
#include "GlobalSessionService.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/ReturnMessage.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"

// std header
#include <thread>
#include <chrono>

GlobalSessionService::GlobalSessionService()
	: ot::ServiceBase(OT_INFO_SERVICE_TYPE_GlobalSessionService, OT_INFO_SERVICE_TYPE_GlobalSessionService),
	m_connectionStatus(Disconnected), m_healthCheckRunning(false), m_workerThread(nullptr)
{

}

GlobalSessionService::~GlobalSessionService() {

}

bool GlobalSessionService::connect(const std::string& _url) {
	{
		std::lock_guard<std::mutex> lock(m_mutex);

		// Ensure connection status is disconnected
		if (m_connectionStatus != Disconnected) {
			OT_LOG_E("GSS state is not Disconnected. Ignoring request for \"" + _url + "\"");
			return false;
		}

		// Clean up potentially running health check thread
		if (m_workerThread) {
			m_healthCheckRunning = false;
			m_workerThread->join();
			delete m_workerThread;
			m_workerThread = nullptr;
		}

		// Update information
		this->setServiceURL(_url);

		m_healthCheckRunning = true;
		m_connectionStatus = CheckingNewConnection;

		OT_LOG_D("Checking for connection to new GDS at \"" + _url + "\"");

		m_workerThread = new std::thread(&GlobalSessionService::healthCheck, this);
	}
	
	// Wait for health check to finish
	int ct = 0;
	const int maxCt = 60;
	while (!this->isConnected()) {
		if (ct++ >= maxCt) {
			OT_LOG_E("Failed to connect to Global Session Service at \"" + _url + "\" after " + std::to_string(maxCt) + " attempts");
			return false;
		}

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	return true;
}

bool GlobalSessionService::isConnected() {
	std::lock_guard<std::mutex> lock(m_mutex);
	return (m_connectionStatus == Connected);
}

bool GlobalSessionService::confirmSession(const std::string& _sessionId, const std::string& _userName) {
	SessionService& lss = SessionService::instance();

	ot::JsonDocument confirmDoc;
	confirmDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_ConfirmSession, confirmDoc.GetAllocator()), confirmDoc.GetAllocator());
	confirmDoc.AddMember(OT_ACTION_PARAM_SESSION_ID, ot::JsonString(_sessionId, confirmDoc.GetAllocator()), confirmDoc.GetAllocator());
	confirmDoc.AddMember(OT_ACTION_PARAM_USER_NAME, ot::JsonString(_userName, confirmDoc.GetAllocator()), confirmDoc.GetAllocator());
	confirmDoc.AddMember(OT_ACTION_PARAM_SERVICE_ID, lss.getId(), confirmDoc.GetAllocator());

	// Send ping
	std::string responseStr;
	if (!ot::msg::send(lss.getUrl(), m_serviceURL, ot::EXECUTE, confirmDoc.toJson(), responseStr, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
		OT_LOG_E("Global session service can not be reached");
		return false;
	}

	ot::ReturnMessage msg = ot::ReturnMessage::fromJson(responseStr);
	if (msg == ot::ReturnMessage::Ok) {
		return true;
	}
	else {
		OT_LOG_E("Failed to confirm session \"" + _sessionId + "\". Message: \"" + msg.getWhat() + "\"");
		return false;
	}
}

void GlobalSessionService::startHealthCheck(void) {
	if (m_workerThread) {
		OTAssert(0, "Health check already running");
		return;
	}

	m_healthCheckRunning = true;

	OT_LOG_D("Starting health check");
	m_workerThread = new std::thread(&GlobalSessionService::healthCheck, this);
}

void GlobalSessionService::stopHealthCheck(bool _joinThread) {
	if (m_workerThread) {
		m_healthCheckRunning = false;
		if (_joinThread) {
			m_workerThread->join();
		}
	}
	else {
		OTAssert(0, "No health check currently running");
	}
}

GSSRegistrationInfo GlobalSessionService::getRegistrationResult(void) {
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_registrationResult;
}

// #################################################################################################################################################

// Private functions

void GlobalSessionService::healthCheck(void) {
	int ct;
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_PING, doc.GetAllocator()), doc.GetAllocator());
	std::string pingMessage = doc.toJson();

	std::string lssUrl = SessionService::instance().getUrl();

	while (m_healthCheckRunning) {
		ct = 0;
		while (ct++ < 60) { // Wait for 1 min
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1s);
		}
		std::string pingResponse;

		// Send ping
		if (!ot::msg::send(lssUrl, m_serviceURL, ot::EXECUTE, pingMessage, pingResponse, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
			OT_LOG_W("Global session service can not be reached at \"" + m_serviceURL + "\"");
			m_connectionStatus = Disconnected;
		}
		// Check response
		else if (pingResponse != OT_ACTION_PING) {
			OT_LOG_W("Received invalid ping response from global session service");
			m_connectionStatus = Disconnected;
		}
		else if (m_connectionStatus != Connected) {

			// Register at the session service
			ot::JsonDocument registerDoc;
			doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_RegisterNewSessionService, doc.GetAllocator()), doc.GetAllocator());
			doc.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(lssUrl, doc.GetAllocator()), doc.GetAllocator());

			doc.AddMember(OT_ACTION_PARAM_Sessions, ot::JsonArray(SessionService::instance().getSessionIds(), doc.GetAllocator()), doc.GetAllocator());

			ot::JsonArray iniListArr;
			doc.AddMember(OT_ACTION_PARAM_IniList, iniListArr, doc.GetAllocator());

			std::string registerResponse;

			// Send registration
			if (!ot::msg::send(lssUrl, m_serviceURL, ot::EXECUTE, registerDoc.toJson(), registerResponse, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
				OT_LOG_E("Failed to send register message to global session service");
			}
			else OT_ACTION_IF_RESPONSE_ERROR(registerResponse) {
				OT_LOG_E(registerResponse);
			}
			else OT_ACTION_IF_RESPONSE_WARNING(registerResponse) {
				OT_LOG_W(registerResponse);
			}
			else {
				// Get new id and database url
				ot::JsonDocument registrationResponseDoc;
				registrationResponseDoc.fromJson(registerResponse);

				GSSRegistrationInfo info;
				info.setServiceID(ot::json::getUInt(registrationResponseDoc, OT_ACTION_PARAM_SERVICE_ID));
				info.setDataBaseURL(ot::json::getString(registrationResponseDoc, OT_ACTION_PARAM_DATABASE_URL));
				info.setAuthURL(ot::json::getString(registrationResponseDoc, OT_ACTION_PARAM_SERVICE_AUTHURL));
				
				if (registrationResponseDoc.HasMember(OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL)) {
					info.setGdsURL(ot::json::getString(registrationResponseDoc, OT_ACTION_PARAM_GLOBALDIRECTORY_SERVICE_URL));
				}

				// Get global log flags if provided
				
				if (registrationResponseDoc.HasMember(OT_ACTION_PARAM_GlobalLogFlags)) {
					ot::ConstJsonArray logFlags = ot::json::getArray(registrationResponseDoc, OT_ACTION_PARAM_GlobalLogFlags);
					ot::LogModeManager logManager;
					logManager.setGlobalLogFlags(ot::logFlagsFromJsonArray(logFlags));
					info.setLogInfo(logManager);
				}

				// Set connection status to connected
				m_connectionStatus = Connected;
			}
		}
	}
}
