// @otlicense

// Auth header
#include "AccessToken/AccessTokenChallenge.h"

// OpenTwin header
#include "OTCore/String.h"
#include "OTSystem/DateTime.h"
#include "OTSystem/RandomNumber/RandomNumberProvider.h"

AccessTokenChallenge AccessTokenChallenge::createNewChallenge(const std::string& _id, const std::string& _token)
{
	std::vector<uint8_t> challengeRaw = ot::RandomNumberProvider::createRandomBytes(32);
	std::string challenge = ot::String::toHexString(challengeRaw);
	return AccessTokenChallenge(_id, _token, std::move(challenge));
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Constructor

AccessTokenChallenge::AccessTokenChallenge(const std::string& _id, const std::string& _token, std::string&& _challenge)
	: m_creationTime(ot::DateTime::msSinceEpoch()), m_id(_id), m_token(_token), m_challenge(std::move(_challenge))
{}