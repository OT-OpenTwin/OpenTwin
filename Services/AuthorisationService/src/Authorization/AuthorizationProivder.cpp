// @otlicense

// Auth header
#include "ServiceBase.h"
#include "Authorization/AuthorizationProvider.h"
#include "SSO/SSOUser.h"
#include "SSO/SSOBuffer.h"

// OpenTwin header
#include "OTCore/JSON/JSON.h"
#include "OTCommunication/ActionTypes.h"
#include "OTServiceFoundation/UserCredentials.h"

AuthorizationProvider::AuthResult AuthorizationProvider::authorizeAction(AuthContext& _authContext, const std::string& _action, const std::string& _loggedInUsername, const ot::ConstJsonObject& _actionDocument)
{
	AuthResult result;
	ServiceBase& serviceBase = ServiceBase::instance();

	ActionAuthenticationType authType = ActionAuthenticationType::EncryptedPassword;

	if (ot::json::exists(_actionDocument, OT_PARAM_AUTH_Token))
	{
		authType = ActionAuthenticationType::SSOToken;
	}
	else if (ot::json::exists(_actionDocument, OT_ACTION_PARAM_SessionToken))
	{
		authType = ActionAuthenticationType::UserSessionToken;
	}
	else if (!ot::json::exists(_actionDocument, OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD))
	{
		throw std::runtime_error("The request does not contain the logged in user password. Please logout and attempt to log in again.");
	}

	//

	switch (authType)
	{
	case ActionAuthenticationType::EncryptedPassword:
	{
		std::string userPw = ot::json::getString(_actionDocument, OT_PARAM_AUTH_LOGGED_IN_USER_PASSWORD);

		// Check whether the provided password is encrypted and decrypt it if needed
		bool pwEncrypted = false;
		if (_actionDocument.HasMember(OT_PARAM_AUTH_ENCRYPTED_PASSWORD))
		{
			pwEncrypted = ot::json::getBool(_actionDocument, OT_PARAM_AUTH_ENCRYPTED_PASSWORD);
		}

		if (pwEncrypted)
		{
			userPw = ot::UserCredentials::decryptString(userPw);
		}

		result.loggedInUserPassword = std::move(userPw);
	}
	break;

	case ActionAuthenticationType::SSOToken:
	{
		SSOBuffer& ssoBuffer = _authContext.getSSOBuffer();

		std::string sessionToken = ot::json::getString(_actionDocument, OT_PARAM_AUTH_Token);
		SSOUser* ssoUser = ssoBuffer.getUser(sessionToken);
		if (ssoUser == nullptr)
		{
			// Most likely a hacking attempt
			assert(0);
			throw ot::Exception::AccessDenied("The provided authentication token is invalid. Please logout and attempt to log in again.");
		}
		else
		{
			const std::string& token = ssoUser->getSessionToken().getToken();

			if (!ssoUser->getSessionToken().tokenIsValid(false))
			{
				// Token is expired. Should have been validated before this request was send.
				assert(0);
				ssoBuffer.removeUser(token);
				throw std::runtime_error("The provided authentication token is expired. Please logout and attempt to log in again.");
			}
		}
	}
	break;

	case ActionAuthenticationType::UserSessionToken:
	{
		ot::UserSessionToken sessionToken(ot::json::getObject(_actionDocument, OT_ACTION_PARAM_SessionToken));
		if (sessionToken.getUserName() != _loggedInUsername)
		{
			// Most likely a hacking attempt
			assert(0);
			throw ot::Exception::AccessDenied("The provided session token is invalid. Please logout and attempt to log in again.");
		}

		UserSessionManager& sessionManager = _authContext.getUserSessionManager();

		if (!sessionManager.isSessionValid(sessionToken))
		{
			// Most likely a hacking attempt
			assert(0);
			throw ot::Exception::AccessDenied("The provided session token is invalid. Please logout and attempt to log in again.");
		}

		result.isAuthorized = true;
		result.loggedInUser = MongoUserFunctions::getUserDataThroughUsername(_loggedInUsername, serviceBase.getAdminClient());
		return result;
	}
	break;

	default:
		assert(0);
		throw ot::Exception::Runtime("Invalid authentication type");
	}

	User loggedInUser(_loggedInUsername);
	std::string loggedInUserPassword = result.loggedInUserPassword.value_or("");

	if (!serviceBase.isAdminUser(loggedInUser))
	{
		std::string originalUserName;
		bool logInSuccessful = MongoUserFunctions::authenticateUser(_loggedInUsername, loggedInUserPassword, serviceBase.getDatabaseURL(), serviceBase.getAdminClient(), originalUserName);

		if (!logInSuccessful)
		{
			assert(0);
			throw std::runtime_error("The User could not be authenticated! Please logout and attempt to log in again!");
		}

		loggedInUser = MongoUserFunctions::getUserDataThroughUsername(originalUserName, serviceBase.getAdminClient());
	}

	result.isAuthorized = true;
	result.loggedInUser = std::move(loggedInUser);

	return result;
}
