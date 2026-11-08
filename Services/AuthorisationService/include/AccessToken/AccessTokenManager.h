// @otlicense

#pragma once

// Auth header
#include "AccessToken/AccessTokenChallengeManager.h"

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
	struct ChallengeData
	{
		
	};
	std::unordered_map<std::string, ChallengeData> m_challenges;
};