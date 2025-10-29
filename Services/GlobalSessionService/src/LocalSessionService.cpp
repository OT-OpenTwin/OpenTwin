// @otlicense

//! @file LocalSessionService.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2022
// ###########################################################################################################################################################################################################################################################################################################################

// GSS header
#include "Session.h"
#include "LocalSessionService.h"

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTCommunication/ActionTypes.h"

// std header
#include <exception>

LocalSessionService::LocalSessionService() : m_id(ot::invalidServiceID) {

}

LocalSessionService::LocalSessionService(const LocalSessionService& _other) :
	m_url(_other.m_url), m_activeSessions(_other.m_activeSessions), m_iniSessions(_other.m_iniSessions), m_id(_other.m_id)
{}

LocalSessionService::LocalSessionService(LocalSessionService&& _other) noexcept :
	m_url(std::move(_other.m_url)), m_activeSessions(std::move(_other.m_activeSessions)), m_iniSessions(std::move(_other.m_iniSessions)), m_id(_other.m_id)
{

}

LocalSessionService::~LocalSessionService() {

}

LocalSessionService& LocalSessionService::operator = (const LocalSessionService& _other) {
	if (this != &_other) {
		m_url = _other.m_url;
		m_activeSessions = _other.m_activeSessions;
		m_iniSessions = _other.m_iniSessions;
		m_id = _other.m_id;
	}

	return *this;
}

LocalSessionService& LocalSessionService::operator=(LocalSessionService&& _other) noexcept {
	if (this != &_other) {
		m_url = std::move(_other.m_url);
		m_activeSessions = std::move(_other.m_activeSessions);
		m_iniSessions = std::move(_other.m_iniSessions);
		m_id = _other.m_id;
	}

	return *this;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Virtual methods

void LocalSessionService::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	// Create active session array
	ot::JsonArray activeArr;
	for (const Session& session : m_activeSessions) {
		ot::JsonObject sessionObj;
		session.addToJsonObject(sessionObj, _allocator);
	}

	// Create initialize session array
	ot::JsonArray iniArr;
	for (const IniSessionType& session : m_iniSessions) {
		ot::JsonObject sessionObj;
		session.second.addToJsonObject(sessionObj, _allocator);

		ot::JsonObject pairObj;
		pairObj.AddMember("T", ot::JsonValue(std::chrono::duration_cast<std::chrono::nanoseconds>(session.first.time_since_epoch()).count()), _allocator);
		pairObj.AddMember("S", sessionObj, _allocator);

		iniArr.PushBack(pairObj, _allocator);
	}

	_object.AddMember(OT_ACTION_PARAM_SERVICE_ID, m_id, _allocator);
	_object.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(m_url, _allocator), _allocator);
	_object.AddMember(OT_ACTION_PARAM_Sessions, activeArr, _allocator);
	_object.AddMember(OT_ACTION_PARAM_IniList, iniArr, _allocator);
}

void LocalSessionService::setFromJsonObject(const ot::ConstJsonObject& _object) {
	this->clearSessions();

	// Get information
	if (_object.HasMember(OT_ACTION_ADD_GROUP_TO_PROJECT)) {
		m_id = ot::json::getUInt(_object, OT_ACTION_PARAM_SERVICE_ID);
	}
	else {
		m_id = ot::invalidServiceID;
	}

	m_url = ot::json::getString(_object, OT_ACTION_PARAM_SERVICE_URL);

	// Get active sessions
	for (const ot::ConstJsonObject& sessionObj : ot::json::getObjectList(_object, OT_ACTION_PARAM_Sessions)) {
		Session session;
		session.setFromJsonObject(sessionObj);
		m_activeSessions.push_back(std::move(session));
	}

	// Get initialize sessions
	for (const ot::ConstJsonObject& pairObj : ot::json::getObjectList(_object, OT_ACTION_PARAM_IniList)) {
		// Get time
		auto time = std::chrono::steady_clock::time_point(std::chrono::nanoseconds(ot::json::getInt(pairObj, "T")));

		// Get session
		Session session;
		session.setFromJsonObject(ot::json::getObject(pairObj, "S"));
		m_iniSessions.push_back(std::make_pair(time, std::move(session)));
	}

}

// ###########################################################################################################################################################################################################################################################################################################################

// Management

void LocalSessionService::addIniSession(Session&& _session) {
	IniSessionType data(std::chrono::steady_clock::now(), std::move(_session));
	m_iniSessions.push_back(std::move(data));
}

std::list<Session> LocalSessionService::checkTimedOutIniSessions(int _timeoutMs) {
	std::list<Session> timedOut;

	// Move the ini list to tmp and clear
	std::list<IniSessionType> tmp = std::move(m_iniSessions);
	m_iniSessions.clear();

	// Get the current time
	auto currentTime = std::chrono::steady_clock::now();

	for (IniSessionType& session : tmp) {
		long long dur = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - session.first).count();
		if (dur > _timeoutMs) {
			// Session timed out
			OT_LOG_D("Session inintialize timeout. Session id: \"" + session.second.getId() + "\"");
			timedOut.push_back(std::move(session.second));
		}
		else {
			// Session did not time out yet, move back to ini list
			m_iniSessions.push_back(std::move(session));
		}
	}

	return timedOut;
}

bool LocalSessionService::confirmSession(const std::string& _sessionId) {
	for (auto it = m_iniSessions.begin(); it != m_iniSessions.end(); it++) {
		if (it->second.getId() == _sessionId) {
			OT_LOG_D("Session confirmed. Id: \"" + _sessionId + "\"");
			m_activeSessions.push_back(std::move(it->second));
			m_iniSessions.erase(it);
			return true;
		}
	}

	OT_LOG_EAS("Session not found in initialize list. Session id: \"" + _sessionId + "\"");
	return false;
}

void LocalSessionService::sessionClosed(const std::string& _sessionId) {
	for (auto it = m_iniSessions.begin(); it != m_iniSessions.end(); it++) {
		if (it->second.getId() == _sessionId) {
			m_iniSessions.erase(it);
			break;
		}
	}

	for (auto it = m_activeSessions.begin(); it != m_activeSessions.end(); it++) {
		if (it->getId() == _sessionId) {
			m_activeSessions.erase(it);
			break;
		}
	}
}

bool LocalSessionService::hasSession(const std::string& _sessionId) const {
	for (const Session& session : m_activeSessions) {
		if (session.getId() == _sessionId) {
			return true;
		}
	}

	for (const IniSessionType& session : m_iniSessions) {
		if (session.second.getId() == _sessionId) {
			return true;
		}
	}

	return false;
}

std::string LocalSessionService::getSessionUser(const std::string& _sessionId) const {
	auto session = this->getSession(_sessionId);
	if (session) {
		return session->getUserName();
	}
	else {
		OT_LOG_E("Session not found. Session Id: \"" + _sessionId + "\"");
		throw std::out_of_range("Session not found. Session Id: \"" + _sessionId + "\"");
	}
}

std::list<std::string> LocalSessionService::getSessionIds(void) const {
	std::list<std::string> result;

	for (const IniSessionType& s : m_iniSessions) {
		result.push_back(s.second.getId());
	}
	for (const Session& s : m_activeSessions) {
		result.push_back(s.getId());
	}

	return result;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

size_t LocalSessionService::getTotalSessionCount(void) const {
	return m_iniSessions.size() + m_activeSessions.size();
}

void LocalSessionService::clearSessions(void) {
	m_activeSessions.clear();
	m_iniSessions.clear();
}

ot::GSSDebugInfo::LSSData LocalSessionService::getDebugInformation() const {
	ot::GSSDebugInfo::LSSData data;

	data.id = m_id;
	data.url = m_url;

	for (const IniSessionType& s : m_iniSessions) {
		data.initializingSessions.push_back(s.second.getDebugInformation());
	}
	for (const Session& s : m_activeSessions) {
		data.activeSessions.push_back(s.getDebugInformation());
	}

	return data;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private

Session* LocalSessionService::getSession(const std::string& _sessionId) {
	for (IniSessionType& session : m_iniSessions) {
		if (session.second.getId() == _sessionId) {
			return &session.second;
		}
	}

	for (Session& session : m_activeSessions) {
		if (session.getId() == _sessionId) {
			return &session;
		}
	}

	return nullptr;
}

const Session* LocalSessionService::getSession(const std::string& _sessionId) const {
	for (const IniSessionType& session : m_iniSessions) {
		if (session.second.getId() == _sessionId) {
			return &session.second;
		}
	}

	for (const Session& session : m_activeSessions) {
		if (session.getId() == _sessionId) {
			return &session;
		}
	}

	return nullptr;
}

