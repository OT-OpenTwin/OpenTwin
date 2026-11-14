// @otlicense
// File: AccessTokenManager.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// Auth header
#include "ServiceBase.h"
#include "AccessToken/AccessToken.h"
#include "AccessToken/AccessTokenManager.h"
#include "MongoDB/MongoUserFunctions.h"

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCrypto/Signature/SignatureProvider.h"

AccessTokenManager::AccessTokenManager()
{

}

AccessTokenManager::AuthenticationResult AccessTokenManager::handleLogin(const ot::ConstJsonObject& _actionDocument, std::string& _username, ot::JsonDocument& _responseDoc)
{
	AuthenticationResult result = AuthenticationResult::Failure;
	
	try
	{
		std::string token = ot::json::getString(_actionDocument, OT_ACTION_PARAM_Token);
		if (token.empty())
		{
			OT_LOG_E("Token is empty");
			return AuthenticationResult::Failure;
		}

		if (_actionDocument.HasMember(OT_ACTION_PARAM_Signature))
		{
			std::string challengeID = ot::json::getString(_actionDocument, OT_ACTION_PARAM_ChallengeID);
			std::string challengeResponse = ot::json::getString(_actionDocument, OT_ACTION_PARAM_Signature);
			if (challengeResponse.empty())
			{
				OT_LOG_E("Challenge response is empty");
				return AuthenticationResult::Failure;
			}

			auto& adminClient = ServiceBase::instance().getAdminClient();
			AccessToken accessToken = AccessToken::readFromDatabase(adminClient, token);

			// Check if the challenge response is valid
			if (m_challengeManager.challengeResponse(accessToken, challengeID, challengeResponse))
			{
				// Retreive user from credentials
				User user = MongoUserFunctions::getUserDataThroughId(accessToken.getUserId(), adminClient);
				_username = user.getName();
				_responseDoc.AddMember(OT_PARAM_AUTH_USERNAME, ot::JsonString(_username, _responseDoc.GetAllocator()), _responseDoc.GetAllocator());

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

