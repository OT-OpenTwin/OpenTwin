// @otlicense
// File: AccessTokenChallengeManager.cpp
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
#include "AccessToken/AccessTokenChallengeManager.h"

// OpenTwin header
#include "OTSystem/RandomNumber/RandomNumberProvider.h"
#include "OTCore/String.h"
#include "OTCore/Logging/Logger.h"

AccessTokenChallengeManager::AccessTokenChallengeManager()
{

}

AccessTokenChallengeManager::~AccessTokenChallengeManager()
{
	if (!m_challenges.empty())
	{
		OTAssert(false, "There are still challenges stored in the manager during destruction. Was this a crash..?");
		OT_LOG_WS("There are still " << m_challenges.size() << " challenges stored in the manager during destruction."
			"\nWas this a crash..?");
	}
}

const AccessTokenChallenge& AccessTokenChallengeManager::challenge(const std::string& _token)
{
	// Generate a new challenge ID
	std::string id = generateId();

	// Generate the challenge
	AccessTokenChallenge challenge = AccessTokenChallenge::createNewChallenge(id, _token);

	// Store challenge
	m_challenges.emplace(id, std::move(challenge));

	return m_challenges.at(id);
}

bool AccessTokenChallengeManager::challengeResponse(const std::string& _id, const std::string& _challenge)
{
	bool valid = false;

	auto it = m_challenges.find(_id);
	if (it != m_challenges.end())
	{
		AccessTokenChallenge& challenge = it->second;
		valid = challenge.getChallenge() == _challenge;
		m_challenges.erase(it);
	}

	return valid;
}

std::string AccessTokenChallengeManager::generateId()
{
	std::string newId;

	do
	{
		auto newIdRaw = ot::RandomNumberProvider::createRandomBytes(16);
		newId = ot::String::toHexString(newIdRaw);
	}
	while (m_challenges.find(newId) != m_challenges.end());

	return newId;
}
