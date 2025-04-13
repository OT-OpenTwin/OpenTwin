//! @file LocalSessionService.h
//! @author Alexander Kuester (alexk95)
//! @date March 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTCore/Serializable.h"
#include "OTCore/OTClassHelper.h"

// std header
#include <list>
#include <string>
#include <chrono>

class Session;

class LocalSessionService : public ot::Serializable {
public:
	LocalSessionService();
	LocalSessionService(const LocalSessionService& _other);
	LocalSessionService(LocalSessionService&& _other) noexcept;
	virtual ~LocalSessionService();

	LocalSessionService& operator = (const LocalSessionService& _other);
	LocalSessionService& operator = (LocalSessionService&& _other) noexcept;

	// ###################################################################

	// Getter / Setter

	void setUrl(const std::string& _url) { m_url = _url; };
	const std::string& getUrl(void) const { return m_url; };

	void setId(ot::serviceID_t _id) { m_id = _id; };
	ot::serviceID_t getId(void) const { return m_id; };

	void addIniSession(Session&& _session);
	
	std::list<Session> checkTimedOutIniSessions(int _timeout);

	//! @brief Confirm the session with the given id.
	//! It will be assumed that the session is in the initialize list.
	//! The confirmed session will be moved to the active session list.
	bool confirmSession(const std::string& _sessionId);

	//const std::list<Session>& getSessions(void) const { return m_sessions; };
	size_t getSessionCount(void) const { return m_sessions.size(); };

	//! @brief Removes the session from the memory.
	//! The session will be searched in all containers.
	void closeSession(const std::string& _sessionId);

	bool hasSession(const std::string& _sessionId) const;

	//! @brief Returns the user of the specified session.
	//! Will throw an exception if the session does not exist.
	std::string getSessionUser(const std::string& _sessionId) const;

	std::list<std::string> getSessionIds(void) const;

	// ###################################################################

	// Data manipulation

	//! @brief Add the object contents to the provided JSON object
	//! @param _object Json object reference
	//! @param _allocator Allocator
	virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

	//! @brief Will set the object contents from the provided JSON object
	//! @param _object The JSON object containing the information
	//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
	virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

	void clear();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private

private:
	//! @brief Returns a pointer to the session with the specified id.
	Session* getSession(const std::string& _sessionId);

	//! @brief Returns a pointer to the session with the specified id.
	const Session* getSession(const std::string& _sessionId) const;


	std::string m_url;
	std::list<Session> m_sessions;
	typedef std::pair<std::chrono::steady_clock::time_point, Session> IniSessionType;
	std::list<IniSessionType> m_iniSessions;
	ot::serviceID_t m_id;
};