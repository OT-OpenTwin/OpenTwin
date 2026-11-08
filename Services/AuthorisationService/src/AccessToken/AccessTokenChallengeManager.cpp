// @otlicense

// Auth header
#include "AccessToken/AccessTokenChallengeManager.h"

// OpenTwin header
#include "OTSystem/RandomNumber/RandomNumberGeneratorCryptoSecure.h"
#include "OTCore/String.h"
#include "OTCore/Logging/Logger.h"

AccessTokenChallengeManager::AccessTokenChallengeManager()
{

}

AccessTokenChallengeManager::~AccessTokenChallengeManager()
{
	if (!m_challenges.empty())
	{
		OTAssert(false, "There are still challenges stored in the manager during destruction. Was this a crash..?");
		OT_LOG_WS("There are still " << m_challenges.size() << " challenges stored in the manager during destruction."
			"\nWas this a crash..?");
	}
}

const AccessTokenChallenge& AccessTokenChallengeManager::challenge(const std::string& _token)
{
	// Generate a new challenge ID
	std::string id = generateId();

	// Generate the challenge
	AccessTokenChallenge challenge = AccessTokenChallenge::createNewChallenge(id, _token);

	// Store challenge
	m_challenges.emplace(id, std::move(challenge));

	return m_challenges.at(id);
}

bool AccessTokenChallengeManager::challengeResponse(const std::string& _id, const std::string& _challenge)
{
	bool valid = false;

	auto it = m_challenges.find(_id);
	if (it != m_challenges.end())
	{
		AccessTokenChallenge& challenge = it->second;
		valid = challenge.getChallenge() == _challenge;
		m_challenges.erase(it);
	}

	return valid;
}

std::string AccessTokenChallengeManager::generateId()
{
	std::string newId;

	do
	{
		auto newIdRaw = ot::RandomNumberGeneratorCryptoSecure().createRandomBytes(16);
		newId = ot::String::toHexString(newIdRaw);
	}
	while (m_challenges.find(newId) != m_challenges.end());

	return newId;
}
