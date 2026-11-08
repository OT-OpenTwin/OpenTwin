// @otlicense

#pragma once

// Auth header
#include "AccessToken/AccessTokenChallenge.h"

// std header
#include <string>
#include <vector>
#include <unordered_map>

class AccessTokenChallengeManager
{
	OT_DECL_NOCOPY(AccessTokenChallengeManager)
	OT_DECL_NOMOVE(AccessTokenChallengeManager)
public:
	AccessTokenChallengeManager();
	~AccessTokenChallengeManager();

	//! @brief Generates a new challenge for the given token and returns the challenge object.
	const AccessTokenChallenge& challenge(const std::string& _token);

	//! @brief Checks if the given challenge response is valid for the challenge with the given ID.
	//! The challenge is removed from the manager after this call, regardless of whether the response is valid or not.
	//! @return True if the challenge response is valid, false otherwise.
	bool challengeResponse(const std::string& _id, const std::string& _challenge);
	
private:
	std::string generateId();

	std::unordered_map<std::string, AccessTokenChallenge> m_challenges;
};