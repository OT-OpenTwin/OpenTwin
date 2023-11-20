#include "SessionService.h"
#include "Session.h"

#include "OTCore/Logger.h"
#include "OTCore/OTAssert.h"
#include "OTCommunication/ActionTypes.h"

SessionService::SessionService() : m_id(ot::invalidServiceID) {

}

SessionService::SessionService(const SessionService& _other) : m_url(_other.m_url), m_id(_other.m_id) {
	for (auto session : _other.m_sessions) {
		m_sessions.push_back(new Session(*session));
	}
}

SessionService::~SessionService() {
	for (auto session : m_sessions) {
		delete session;
	}
}

SessionService& SessionService::operator = (const SessionService& _other) {
	m_url = _other.m_url;
	m_id = _other.m_id;
	for (auto session : _other.m_sessions) {
		m_sessions.push_back(new Session(*session));
	}
	return *this;
}

bool SessionService::addSession(const Session& _session) {
	m_sessions.push_back(new Session(_session));
	return true;
}

void SessionService::removeSession(Session * _session) {
	auto it = std::find(m_sessions.begin(), m_sessions.end(), _session);
	if (it != m_sessions.end()) {
		delete _session;
		m_sessions.erase(it);
	}
}

size_t SessionService::sessionCount(void) const { return m_sessions.size(); }

Session * SessionService::getSessionById(const std::string& _sessionId) {
	for (auto s : m_sessions) {
		if (s->ID() == _sessionId) return s;
	}
	return nullptr;
}

// ###################################################################

// Data manipulation

bool SessionService::setFromDocument(OT_rJSON_doc& _document) {
	// Check document types
	if (!_document.IsObject()) {
		OT_LOG_E("Provided document is not an object");
		OTAssert(0, "Provided document is not an object");
		return false;
	}
	if (!_document.HasMember(OT_ACTION_PARAM_SERVICE_URL)) {
		OT_LOG_E("Action member missing: " OT_ACTION_PARAM_SERVICE_URL);
		OTAssert(0, "Action member missing: " OT_ACTION_PARAM_SERVICE_URL);
		return false;
	}
	if (!_document.HasMember(OT_ACTION_PARAM_SESSION_LIST)) {
		OT_LOG_E("Action member missing: " OT_ACTION_PARAM_SESSION_LIST);
		OTAssert(0, "Action member missing: " OT_ACTION_PARAM_SESSION_LIST);
		return false;
	}
	if (!_document[OT_ACTION_PARAM_SESSION_LIST].IsArray()) {
		OT_LOG_E("Action member wrong data type: " OT_ACTION_PARAM_SERVICE_URL);
		OTAssert(0, "Action member wrong data type: " OT_ACTION_PARAM_SERVICE_URL);
		return false;
	}

	// Get information
	std::string url = ot::rJSON::getString(_document, OT_ACTION_PARAM_SERVICE_URL);
	std::list<std::string> sessionIds = ot::rJSON::getStringList(_document, OT_ACTION_PARAM_SESSION_LIST);

	clear();
	m_url = url;
	for (auto id : sessionIds) {
		m_sessions.push_back(new Session(id));
	}

	return true;
}

void SessionService::clear(void) {
	for (auto s : m_sessions) delete s;
	m_sessions.clear();
}