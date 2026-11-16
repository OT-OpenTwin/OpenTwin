// @otlicense

// Auth header
#include "UserSession/UserSessionManager.h"

// OpenTwin header
#include "OTSystem/RandomNumber/RandomNumberProvider.h"
#include "OTCore/String.h"
#include "OTCrypto/CryptoDefaults.h"

UserSessionManager::UserSessionManager()
{

}

std::optional<std::string> UserSessionManager::createUserSession(const User& _user)
{
	std::vector<uint8_t> randomBytes = ot::RandomNumberProvider::createRandomBytes(ot::CryptoDefaults::userSessionTokenLength());
	std::string sessionToken = ot::String::compressBase64(randomBytes.data(), randomBytes.size());

}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Session management

void UserSessionManager::forgetUserSession(const std::string& _userID, const std::string& _sessionToken)
{
	auto userSessionIt = m_userToSessions.find(_userID);
	if (userSessionIt != m_userToSessions.end())
	{
		auto& sessionList = userSessionIt->second;
		sessionList.remove(_sessionToken);
		if (sessionList.empty())
		{
			m_userToSessions.erase(userSessionIt);
		}
	}
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
			if (!it->second.isValid())
			{
				// Remove expired session
				forgetUserSession(it->second.getUser().getID(), it->first);
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

	uint32_t sleepCount = cleanupInterval / sleepInterval;
	
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
