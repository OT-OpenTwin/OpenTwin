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

//! @class LocalSessionService
//! @brief The LocalSessionService class contains all relevant information about a LSS.
//! Sessions added to the LocalSessionService can be in one of the following states:
//!		- Initialize: The session was created but not confirmed yet.
//! 	- Active: The session was confirmed and is concidered open while the LSS is alive and did not tell otherwise.
class LocalSessionService : public ot::Serializable {
public:
	LocalSessionService();
	LocalSessionService(const LocalSessionService& _other);
	LocalSessionService(LocalSessionService&& _other) noexcept;
	virtual ~LocalSessionService();

	LocalSessionService& operator = (const LocalSessionService& _other);
	LocalSessionService& operator = (LocalSessionService&& _other) noexcept;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Management

	//! @brief Add the session to the list of sessions that wait for confirmation.
	//! The current timestamp will be saved to check the timeout later.
	//! @param _session The session to add.
	//! @ref Session::checkTimedOutIniSessions
	void addIniSession(Session&& _session);
	
	//! @brief Check the sessions in the initialize list for timeout.
	//! Every timed out session will be taken from the initialize list and returned.
	//! This is used to prevent sessions from being locked forever if the LSS is not reachable anymore
	//! after the session was created but yet not confirmed.
	//! @param _timeoutMs Timeout in milliseconds.
	std::list<Session> checkTimedOutIniSessions(int _timeoutMs);

	//! @brief Confirm the session with the given id.
	//! It will be assumed that the session is in the initialize list.
	//! The confirmed session will be moved to the active session list.
	//! @param _sessionId Session id to confirm.
	//! @ref Session::checkTimedOutIniSessions
	bool confirmSession(const std::string& _sessionId);

	//! @brief Removes the session from the memory.
	//! The session will be removed from all containers.
	//! @param _sessionId Session id to remove.
	void closeSession(const std::string& _sessionId);

	//! @brief Returns true if the session with the specified was assigned to this LSS.
	//! The session will be searched in all containers.
	//! @param _sessionId Session id to check.
	bool hasSession(const std::string& _sessionId) const;

	//! @brief Returns the user of the specified session.
	//! Will throw an exception if the session does not exist.
	//! @param _sessionId Session id to check.
	std::string getSessionUser(const std::string& _sessionId) const;

	//! @brief Returns a list of all session ids assigned to this LSS.
	//! The list will contain the session ids of all sessions in all containers.
	std::list<std::string> getSessionIds(void) const;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Getter / Setter

	//! @brief Set the LocalSessionService id.
	//! @param _id ID to set.
	void setId(ot::serviceID_t _id) { m_id = _id; };

	//! @brief Returns the LocalSessionService id.
	ot::serviceID_t getId(void) const { return m_id; };

	//! @brief Set the LocalSessionService url.
	//! @param _url Url to set.
	void setUrl(const std::string& _url) { m_url = _url; };

	//! @brief Returns the LocalSessionService url.
	const std::string& getUrl(void) const { return m_url; };

	//! @brief Returns the total number of sessions assigned to this LSS.
	//! The total number is the number of sessions in the initialize list and the active list.
	size_t getTotalSessionCount(void) const;

	//! @brief Clears all containers that hold sessions.
	void clearSessions(void);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Serialization

	virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

	virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private

private:
	//! @brief Returns a pointer to the session with the specified id.
	Session* getSession(const std::string& _sessionId);

	//! @brief Returns a pointer to the session with the specified id.
	const Session* getSession(const std::string& _sessionId) const;

	//! @brief Type used to store the initialize sessions.
	typedef std::pair<std::chrono::steady_clock::time_point, Session> IniSessionType;

	ot::serviceID_t m_id;                    //! @brief The id of the LocalSessionService.
	std::string m_url;                       //! @brief The url of the LocalSessionService.
	std::list<Session> m_activeSessions;     //! @brief The list of active sessions.
	std::list<IniSessionType> m_iniSessions; //! @brief The list of sessions that are waiting for confirmation.
};