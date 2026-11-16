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

	//! @brief Authorizes an action based on the provided action type, logged in username, and action document.
	//! @param _action The action type to authorize.
	//! @param _loggedInUsername The username of the logged in user.
	//! @param _actionDocument The action document containing the required credentials and the rest of the action data.
	//! @return true if the action is authorized, false otherwise.
	//! @throws ot::AccessDeniedException if the action cannot be authorized.
	OT_DECL_NODISCARD AuthResult authorizeAction(AuthContext& _authContext, const std::string& _action, const std::string& _loggedInUsername, const ot::ConstJsonObject& _actionDocument);
};