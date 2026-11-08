// @otlicense

#pragma once

// Auth header
#include "AccessToken/AccessTokenChallengeManager.h"
#include "Management/User.h"

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTCore/JSON/JSON.h"

// std header
#include <unordered_map>

class AccessTokenManager
{
	OT_DECL_NOCOPY(AccessTokenManager)
	OT_DECL_NOMOVE(AccessTokenManager)
public:
	enum class AuthenticationResult
	{
		Success,
		ChallengeRequired,
		Failure
	};

	AccessTokenManager();

	AuthenticationResult handleLogin(const ot::ConstJsonObject& _actionDocument, std::string& _username, std::string& _password, ot::JsonDocument& _responseDoc);

private:
	User getUserByToken(const std::string& _token);

	std::string decryptChallenge(const std::string& _token, const std::string& _challenge);

	AccessTokenChallengeManager m_challengeManager;
};