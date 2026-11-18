// @otlicense

#pragma once

// Auth header
#include "UserSession/UserSession.h"

// OpenTwin header
#include "OTCommunication/Token/UserSessionToken.h"

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

	struct UserSessionInfo
	{
		ot::UserSessionToken sessionToken;
		std::string sessionUserName;
		std::string sessionUserPWD;
	};
	UserSessionInfo createUserSession(const std::string& _loggedInUserName);

	bool isSessionValid(const ot::UserSessionToken& _token);

private:
	std::mutex m_mutex;
	std::atomic_bool m_stopCleanupThread;
	std::unique_ptr<std::thread> m_cleanupThread;

	int64_t m_sessionValidityInSeconds = 3600; // Default session validity: 1 hour

	typedef std::unordered_map<std::string, std::list<std::string>> UserNameToSessionNamesMap;
	UserNameToSessionNamesMap m_userToSessions;

	typedef std::unordered_map<std::string, UserSession> SessionNameToSessionMap;
	SessionNameToSessionMap m_sessions;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Session management

	void forgetUserSession(const std::string& _userName, const std::string& _sessionName);
	std::list<std::string>& findOrCreateSessionListForUser(const std::string& _userName);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Worker

	void cleanupWorker();
	bool nextCleanupWorkerCycle() const;
};