// @otlicense
// File: AccessTokenChallengeManager.h
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