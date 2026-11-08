// @otlicense

// Auth header
#include "AccessToken/AccessTokenManager.h"

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTCommunication/ActionTypes.h"

AccessTokenManager::AccessTokenManager()
{

}

AccessTokenManager::AuthenticationResult AccessTokenManager::handleLogin(const ot::ConstJsonObject& _actionDocument, std::string& _username, std::string& _password, ot::JsonDocument& _responseDoc)
{
	AuthenticationResult result = AuthenticationResult::Failure;

	try
	{
		std::string token = ot::json::getString(_actionDocument, OT_ACTION_PARAM_Token);

		if (_actionDocument.HasMember(OT_ACTION_PARAM_Challenge))
		{
			std::string challengeID = ot::json::getString(_actionDocument, OT_ACTION_PARAM_ChallengeID);
			std::string challengeResponse = ot::json::getString(_actionDocument, OT_ACTION_PARAM_Challenge);
			std::string decryptedChallenge = decryptChallenge(token, challengeResponse);

			// Check if the challenge response is valid
			if (m_challengeManager.challengeResponse(challengeID, decryptedChallenge))
			{
				// Challenge response is valid, proceed with login
				_username = token; // For demonstration purposes, we use the token as the username
				_password = decryptedChallenge; // For demonstration purposes, we use the decrypted challenge as the password
				result = AuthenticationResult::Success;
			}
			else
			{
				// Challenge response is invalid
				result = AuthenticationResult::Failure;
			}
		}
		else
		{
			// Create new challenge for the given token
			const auto& challenge = m_challengeManager.challenge(token);
			
			_responseDoc.AddMember(OT_ACTION_PARAM_ChallengeID, ot::JsonString(challenge.getID(), _responseDoc.GetAllocator()), _responseDoc.GetAllocator());
			_responseDoc.AddMember(OT_ACTION_PARAM_Challenge, ot::JsonString(challenge.getChallenge(), _responseDoc.GetAllocator()), _responseDoc.GetAllocator());

			result = AuthenticationResult::ChallengeRequired;
		}
	}
	catch (const std::exception& e)
	{
		OT_LOG_E("Exception during login: " + std::string(e.what()));
		result = AuthenticationResult::Failure;
	}
	catch (...)
	{
		OT_LOG_E("Unknown exception during login");
		result = AuthenticationResult::Failure;
	}

	return result;
}

User AccessTokenManager::getUserByToken(const std::string& _token)
{
	return User();
}

std::string AccessTokenManager::decryptChallenge(const std::string& _token, const std::string& _challenge)
{
	return "";
}
