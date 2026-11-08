// @otlicense

// Auth header
#include "AccessToken/AccessTokenChallenge.h"

// OpenTwin header
#include "OTSystem/DateTime.h"

AccessTokenChallenge AccessTokenChallenge::createNewChallenge(const std::string& _id, const std::string& _token)
{
	std::string challenge;
	return AccessTokenChallenge(_id, _token, std::move(challenge));
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Constructor

AccessTokenChallenge::AccessTokenChallenge(const std::string& _id, const std::string& _token, std::string&& _challenge)
	: m_creationTime(ot::DateTime::msSinceEpoch()), m_id(_id), m_token(_token), m_challenge(std::move(_challenge))
{

}