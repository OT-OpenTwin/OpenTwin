// @otlicense

#pragma once

// Auth header
#include "Authorization/AuthContext.h"
#include "Management/User.h"

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTCore/JSON/JSONTypes.h"

class AuthorizationProvider
{
	OT_DECL_NOCOPY(AuthorizationProvider)
	OT_DECL_NOMOVE(AuthorizationProvider)
public:
	enum class ActionAuthenticationType
	{
		EncryptedPassword,
		SSOToken,
		UserSessionToken
	};

	struct AuthResult
	{
		bool isAuthorized = false;
		std::optional<std::string> reason;
		std::optional<User> loggedInUser;
		std::optional<std::string> loggedInUserPassword;
	};

	AuthorizationProvider() = default;
	~AuthorizationProvider() = default;

	OT_DECL_NODISCARD AuthResult authorizeAction(AuthContext& _authContext, const std::string& _action, std::string& _loggedInUsername, const ot::ConstJsonObject& _actionDocument);
};