// @otlicense
// File: AccessTokenChallenge.cpp
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
#include "AccessToken/AccessTokenChallenge.h"

// OpenTwin header
#include "OTCore/String.h"
#include "OTSystem/DateTime.h"
#include "OTSystem/RandomNumber/RandomNumberProvider.h"

AccessTokenChallenge AccessTokenChallenge::createNewChallenge(const std::string& _id, const std::string& _token)
{
	std::vector<uint8_t> challengeRaw = ot::RandomNumberProvider::createRandomBytes(32);
	std::string challenge = ot::String::toHexString(challengeRaw);
	return AccessTokenChallenge(_id, _token, std::move(challenge));
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Constructor

AccessTokenChallenge::AccessTokenChallenge(const std::string& _id, const std::string& _token, std::string&& _challenge)
	: m_creationTime(ot::DateTime::msSinceEpoch()), m_id(_id), m_token(_token), m_challenge(std::move(_challenge))
{}