#include "GlobalSessionService.h"
#include "SessionService.h"
#include "Session.h"

#include "OTCore/CoreTypes.h"
#include "OTCore/OTAssert.h"
#include "OTCore/Logger.h"
#include "OTCore/rJSON.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"

#include <thread>
#include <chrono>

GlobalSessionService::GlobalSessionService(const std::string& _url, SessionService * _owner)
	: ot::ServiceBase(OT_INFO_SERVICE_TYPE_GlobalSessionService, OT_INFO_SERVICE_TYPE_GlobalSessionService, _url, ot::invalidServiceID),
	m_owner(_owner), m_connectionStatus(Connected), m_healthCheckRunning(false), m_workerThread(nullptr)
{

}

GlobalSessionService::~GlobalSessionService() {

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

// #################################################################################################################################################

// Private functions

void GlobalSessionService::healthCheck(void) {
	int ct;
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_PING);
	std::string pingMessage = ot::rJSON::toJSON(doc);

	while (m_healthCheckRunning) {
		ct = 0;
		while (ct++ < 60) { // Wait for 1 min
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1s);
		}
		std::string response;

		// Send ping
		if (!ot::msg::send(m_owner->url(), m_serviceURL, ot::EXECUTE, pingMessage, response)) {
			OT_LOG_W("Global session service can not be reached");
			m_connectionStatus = ConnectionFailed;
		}
		// Check response
		else if (response != OT_ACTION_PING) {
			OT_LOG_W("Received invalid ping response from global session service");
			m_connectionStatus = ConnectionFailed;
		}
		// Check if last ping was not successful
		else if (m_connectionStatus == ConnectionFailed) {
			// Register again at the session service
			OT_rJSON_createDOC(registerDoc);
			ot::rJSON::add(registerDoc, OT_ACTION_MEMBER, OT_ACTION_CMD_RegisterNewSessionService);
			ot::rJSON::add(registerDoc, OT_ACTION_PARAM_SERVICE_URL, m_owner->url());

			OT_rJSON_createValueArray(sessionList);
			for (auto s : m_owner->sessions()) {
				OT_rJSON_val sessionID;
				sessionList.PushBack(OT_rJSON_val(s->id().c_str(), registerDoc.GetAllocator()), registerDoc.GetAllocator());
			}
			ot::rJSON::add(registerDoc, OT_ACTION_PARAM_SESSION_LIST, sessionList);

			response.clear();
			// Send registration
			if (!ot::msg::send(m_owner->url(), m_serviceURL, ot::EXECUTE, ot::rJSON::toJSON(registerDoc), response)) {
				OT_LOG_E("Failed to send register message to global session service");
			}
			else OT_ACTION_IF_RESPONSE_ERROR(response) {
				OT_LOG_E(response);
			}
			else OT_ACTION_IF_RESPONSE_WARNING(response) {
				OT_LOG_W(response);
			}
			else {
				// Get new id and database url
				OT_rJSON_parseDOC(registrationResponseDoc, response.c_str());
				ot::serviceID_t ssID = ot::rJSON::getUInt(registrationResponseDoc, OT_ACTION_PARAM_SERVICE_ID);
				std::string databaseURL = ot::rJSON::getString(registrationResponseDoc, OT_ACTION_PARAM_DATABASE_URL);
				m_owner->setId(ssID);
				m_owner->setDataBaseURL(databaseURL);
				
				m_connectionStatus = Connected;
			}
		}
	}
}