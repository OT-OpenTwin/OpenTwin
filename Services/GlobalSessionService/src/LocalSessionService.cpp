//! @file LocalSessionService.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2022
// ###########################################################################################################################################################################################################################################################################################################################

// GSS header
#include "Session.h"
#include "LocalSessionService.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/OTAssert.h"
#include "OTCommunication/ActionTypes.h"

LocalSessionService::LocalSessionService() : m_id(ot::invalidServiceID) {

}

LocalSessionService::LocalSessionService(const LocalSessionService& _other) : m_url(_other.m_url), m_id(_other.m_id) {
	for (auto session : _other.m_sessions) {
		m_sessions.push_back(new Session(*session));
	}
}

LocalSessionService::~LocalSessionService() {
	for (auto session : m_sessions) {
		delete session;
	}
}

LocalSessionService& LocalSessionService::operator = (const LocalSessionService& _other) {
	m_url = _other.m_url;
	m_id = _other.m_id;
	for (auto session : _other.m_sessions) {
		m_sessions.push_back(new Session(*session));
	}
	return *this;
}

void LocalSessionService::addSession(const Session& _session) {
	m_sessions.push_back(new Session(_session));
}

void LocalSessionService::removeSession(Session * _session) {
	auto it = std::find(m_sessions.begin(), m_sessions.end(), _session);
	if (it != m_sessions.end()) {
		delete _session;
		m_sessions.erase(it);
	}
}

Session* LocalSessionService::getSessionById(const std::string& _sessionId) {
	for (auto s : m_sessions) {
		if (s->getId() == _sessionId) return s;
	}
	return nullptr;
}

// ###################################################################

// Data manipulation

void LocalSessionService::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	std::list<std::string> sessionArr;
	for (auto s : m_sessions) {
		sessionArr.push_back(s->getId());
	}
	_object.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(m_url, _allocator), _allocator);
	_object.AddMember(OT_ACTION_PARAM_SESSION_LIST, ot::JsonArray(sessionArr, _allocator), _allocator);
}

void LocalSessionService::setFromJsonObject(const ot::ConstJsonObject& _object) {
	// Get information
	m_url = ot::json::getString(_object, OT_ACTION_PARAM_SERVICE_URL);
	std::list<std::string> sessionIds = ot::json::getStringList(_object, OT_ACTION_PARAM_SESSION_LIST);

	clear();
	for (auto id : sessionIds) {
		m_sessions.push_back(new Session(id));
	}
}

void LocalSessionService::clear(void) {
	for (auto s : m_sessions) delete s;
	m_sessions.clear();
}