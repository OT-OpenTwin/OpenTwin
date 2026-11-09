// @otlicense
// File: SSOSessionToken.cpp
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
#include "SSO/SSOSessionToken.h"

// OpenTwin header
#include "OTSystem/RandomNumber/RandomNumberProvider.h"
#include "OTCore/String.h"

std::string ot::SSOSessionToken::generateToken()
{
	uint64_t tokenBase = RandomNumberProvider::createPositiveRandomInt64();
	
	auto* ptr = reinterpret_cast<const uint8_t*>(&tokenBase);
	std::vector<uint8_t> bytes(ptr, ptr + sizeof(tokenBase));
	m_token = ot::String::compressBase64(bytes.data(), bytes.size());
	
	m_creationTime = std::chrono::steady_clock::now();

	return m_token;
}

const std::string& ot::SSOSessionToken::getToken() const
{
	return m_token;
}

bool ot::SSOSessionToken::tokenIsValid(bool _useDelta) const
{
	auto now = std::chrono::steady_clock::now();
	std::chrono::minutes passedTime = std::chrono::duration_cast<std::chrono::minutes>(now - m_creationTime);
	// The UI may first request the validity of a token and afterwards send a request alongside the token. Between these two requests, the token may become invalid if there is not a delta between the actual validation timeout and the request for validation
	bool valid;
	if (_useDelta)
	{
		valid = passedTime < (m_tokenValidityDurationAbsolute - m_tokenValidityDelta); 
	}
	else
	{
		valid = passedTime < (m_tokenValidityDurationAbsolute);
	}
	return valid;
}
