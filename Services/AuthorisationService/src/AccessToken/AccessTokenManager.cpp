// @otlicense

// Auth header
#include "AccessToken/AccessTokenManager.h"

// OpenTwin header
#include "OTCommunication/ActionTypes.h"

AccessTokenManager::AccessTokenManager()
{

}

AccessTokenManager::AuthenticationResult AccessTokenManager::handleLogin(const ot::ConstJsonObject& _actionDocument, std::string& _username, std::string& _password, ot::JsonDocument& _responseDoc)
{
	std::string token = ot::json::getString(_actionDocument, OT_ACTION_PARAM_Token);

	if (_actionDocument.HasMember(OT_ACTION_PARAM_Challenge))
	{

	}
	else
	{

	}

	return AuthenticationResult::Failure;
}
