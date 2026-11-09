// @otlicense
// File: AccessTokenManager.h
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