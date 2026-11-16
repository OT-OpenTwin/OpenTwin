// @otlicense

#pragma once

// Auth header
#include "UserSession/UserSession.h"

// std header
#include <mutex>
#include <thread>
#include <optional>
#include <unordered_map>

class UserSessionManager
{
	OT_DECL_NOCOPY(UserSessionManager)
	OT_DECL_NOMOVE(UserSessionManager)
public:
	UserSessionManager();

	std::optional<std::string> createUserSession(const User& _user);

private:
	std::mutex m_mutex;
	std::atomic_bool m_stopCleanupThread;
	std::unique_ptr<std::thread> m_cleanupThread;

	int64_t m_sessionValidityInSeconds = 3600; // Default session validity: 1 hour

	typedef std::unordered_map<std::string, std::list<std::string>> UserIDToSessionIDsMap;
	UserIDToSessionIDsMap m_userToSessions;

	typedef std::unordered_map<std::string, UserSession> SessionIDToSessionMap;
	SessionTokenToSessionMap m_sessions;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Session management

	void forgetUserSession(const std::string& _userID, const std::string& _sessionToken);
	std::list<std::string>& findOrCreateSessionListForUser(const std::string& _userID);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Worker

	void cleanupWorker();
	bool nextCleanupWorkerCycle() const;
};