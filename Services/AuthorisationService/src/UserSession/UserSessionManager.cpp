// @otlicense

// Auth header
#include "ServiceBase.h"
#include "UserSession/UserSessionManager.h"
#include "MongoDB/MongoSessionFunctions.h"

// OpenTwin header
#include "OTSystem/RandomNumber/RandomNumberProvider.h"
#include "OTCore/String.h"
#include "OTCrypto/CryptoDefaults.h"

UserSessionManager::UserSessionManager()
{

}

UserSessionManager::UserSessionInfo UserSessionManager::createUserSession(const std::string& _loggedInUsername)
{
	ServiceBase& serviceBase = ServiceBase::instance();

	// Create a new session name for the user
	std::string sessionName = MongoSessionFunctions::createSession(_loggedInUsername, serviceBase.getAdminClient());

	std::vector<uint8_t> randomBytes = ot::RandomNumberProvider::createRandomBytes(ot::CryptoDefaults::userSessionTokenLength());
	std::string sessionToken = ot::String::toHexString(randomBytes);

	randomBytes = ot::RandomNumberProvider::createRandomBytes(ot::CryptoDefaults::userSessionPasswordLength());
	std::string sessionPWD = ot::String::toHexString(randomBytes);

	User user = MongoUserFunctions::getUserDataThroughUsername(_loggedInUsername, serviceBase.getAdminClient());

	// Create temporary user in the database for this session
	auto newTmpUser = MongoUserFunctions::createTmpUser(sessionName, sessionPWD, user, serviceBase.getDatabaseURL(), serviceBase.getAdminClient());

	// Store session information
	UserSession newSession(sessionName, sessionToken, m_sessionValidityInSeconds);
	newSession.setLoggedInUserName(user.getName());
	newSession.setSessionUserName(newTmpUser.userName);
	newSession.setSessionUserPWD(newTmpUser.userPWD);
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		m_sessions.emplace(sessionName, std::move(newSession));
		findOrCreateSessionListForUser(user.getName()).push_back(sessionName);
	}

	ot::UserSessionToken sessionTokenObj(user.getName(), sessionName, sessionToken);

	UserSessionInfo result;
	result.sessionToken = std::move(sessionTokenObj);
	result.sessionUserName = newTmpUser.userName;
	result.sessionUserPWD = newTmpUser.userPWD;
	return result;
}

bool UserSessionManager::isSessionValid(const ot::UserSessionToken& _token)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	auto sessionIt = m_sessions.find(_token.getSessionName());
	if (sessionIt == m_sessions.end())
	{
		return false;
	}

	auto& session = sessionIt->second;
	if (session.isExpired())
	{
		// Remove expired session
		forgetUserSession(session.getLoggedInUserName(), session.getName());
		m_sessions.erase(sessionIt);

		return false;
	}

	return session.getToken() == _token.getToken()
		&& session.getLoggedInUserName() == _token.getUserName()
		&& session.getName() == _token.getSessionName();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Session management

void UserSessionManager::forgetUserSession(const std::string& _userName, const std::string& _sessionName)
{
	auto userSessionIt = m_userToSessions.find(_userName);
	if (userSessionIt != m_userToSessions.end())
	{
		auto& sessionList = userSessionIt->second;
		sessionList.remove(_sessionName);
		if (sessionList.empty())
		{
			m_userToSessions.erase(userSessionIt);
		}
	}
}

std::list<std::string>& UserSessionManager::findOrCreateSessionListForUser(const std::string& _userName)
{
	auto userSessionIt = m_userToSessions.find(_userName);
	if (userSessionIt == m_userToSessions.end())
	{
		auto result = m_userToSessions.emplace(_userName, std::list<std::string>{});
		userSessionIt = result.first;
	}
	return userSessionIt->second;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Worker

void UserSessionManager::cleanupWorker()
{
	while (nextCleanupWorkerCycle())
	{
		std::lock_guard<std::mutex> lock(m_mutex);
		for (auto it = m_sessions.begin(); it != m_sessions.end(); )
		{
			if (it->second.isExpired())
			{
				// Remove expired session
				forgetUserSession(it->second.getLoggedInUserName(), it->first);
				it = m_sessions.erase(it);
			}
			else
			{
				// Move to the next session
				it++;
			}
		}
	}
}

bool UserSessionManager::nextCleanupWorkerCycle() const
{
	constexpr std::chrono::milliseconds cleanupInterval(5 * 60 * 1000); // 5 minutes
	constexpr std::chrono::milliseconds sleepInterval(5 * 1000); // 5 seconds

	uint32_t sleepCount = static_cast<uint32_t>(cleanupInterval / sleepInterval);
	
	for (uint32_t i = 0; i < sleepCount; ++i)
	{
		if (!m_stopCleanupThread.load())
		{
			std::this_thread::sleep_for(sleepInterval);
		}
		else
		{
			return false;
		}
	}

	bool continueWorker = !m_stopCleanupThread.load();
	return continueWorker;
}
