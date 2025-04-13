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

// std header
#include <exception>

LocalSessionService::LocalSessionService() : m_id(ot::invalidServiceID) {

}

LocalSessionService::LocalSessionService(const LocalSessionService& _other) :
	m_url(_other.m_url), m_id(_other.m_id) 
{
	for (const Session& session : _other.m_sessions) {
		m_sessions.push_back(Session(session));
	}
}

LocalSessionService::~LocalSessionService() {

}

LocalSessionService& LocalSessionService::operator = (const LocalSessionService& _other) {
	m_url = _other.m_url;
	m_id = _other.m_id;
	for (const Session& session : _other.m_sessions) {
		m_sessions.push_back(Session(session));
	}
	return *this;
}

void LocalSessionService::addIniSession(Session&& _session) {
	IniSessionType data(std::chrono::steady_clock::now(), std::move(_session));
	m_iniSessions.push_back(std::move(data));
}

std::list<Session> LocalSessionService::checkTimedOutIniSessions(int _timeout) {
	std::list<Session> timedOut;

	// Move the ini list to tmp and clear
	std::list<IniSessionType> tmp = std::move(m_iniSessions);
	m_iniSessions.clear();

	// Get the current time
	auto currentTime = std::chrono::steady_clock::now();

	for (IniSessionType& session : tmp) {
		if (_timeout < std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - session.first).count()) {
			// Session timed out
			OT_LOG_E("Session inintialize timeout. Session id: \"" + session.second.getId() + "\"");
			timedOut.push_back(std::move(session.second));
		}
		else {
			// Session did not time out yet, move back to ini list
			m_iniSessions.push_back(std::move(session));
		}
	}

	return timedOut;
}

void LocalSessionService::confirmSession(const std::string& _sessionId) {
	for (auto it = m_iniSessions.begin(); it != m_iniSessions.end(); it++) {
		if (it->second.getId() == _sessionId) {
			OT_LOG_D("Session confirmed. Id: \"" + _sessionId + "\"");
			m_sessions.push_back(std::move(it->second));
			m_iniSessions.erase(it);
			return;
		}
	}

	OT_LOG_EAS("Session not found in initialize list. Session id: \"" + _sessionId + "\"");
}

void LocalSessionService::closeSession(const std::string& _sessionId) {
	for (auto it = m_iniSessions.begin(); it != m_iniSessions.end(); it++) {
		if (it->second.getId() == _sessionId) {
			m_iniSessions.erase(it);
			break;
		}
	}

	for (auto it = m_sessions.begin(); it != m_sessions.end(); it++) {
		if (it->getId() == _sessionId) {
			m_sessions.erase(it);
			break;
		}
	}
}

bool LocalSessionService::hasSession(const std::string& _sessionId) const {
	for (const Session& session : m_sessions) {
		if (session.getId() == _sessionId) {
			return true;
		}
	}

	return false;
}

std::string LocalSessionService::getSessionUser(const std::string& _sessionId) const {
	auto session = this->getSession(_sessionId);
	if (session) {
		return session->getId();
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
	for (const Session& s : m_sessions) {
		result.push_back(s.getId());
	}

	return result;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Data manipulation

void LocalSessionService::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	std::list<std::string> sessionArr;
	for (const Session& s : m_sessions) {
		sessionArr.push_back(s.getId());
	}
	_object.AddMember(OT_ACTION_PARAM_SERVICE_URL, ot::JsonString(m_url, _allocator), _allocator);
	_object.AddMember(OT_ACTION_PARAM_SESSION_LIST, ot::JsonArray(sessionArr, _allocator), _allocator);
}

void LocalSessionService::setFromJsonObject(const ot::ConstJsonObject& _object) {
	// Get information
	m_url = ot::json::getString(_object, OT_ACTION_PARAM_SERVICE_URL);
	std::list<std::string> sessionIds = ot::json::getStringList(_object, OT_ACTION_PARAM_SESSION_LIST);

	this->clear();

	for (const std::string& id : sessionIds) {
		m_sessions.push_back(Session(id));
	}
}

void LocalSessionService::clear(void) {
	m_sessions.clear();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private

Session* LocalSessionService::getSession(const std::string& _sessionId) {
	for (IniSessionType& session : m_iniSessions) {
		if (session.second.getId() == _sessionId) {
			return &session.second;
		}
	}

	for (Session& session : m_sessions) {
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

	for (const Session& session : m_sessions) {
		if (session.getId() == _sessionId) {
			return &session;
		}
	}

	return nullptr;
}

