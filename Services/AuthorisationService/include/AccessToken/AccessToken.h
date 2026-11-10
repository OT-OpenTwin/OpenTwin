// @otlicense
// File: AccessToken.h
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

// OpenTwin header
#include "OTCore/CoreTypes.h"

// MongoDB header
#include <bsoncxx/document/view.hpp>
#include <mongocxx/client.hpp>

// std header
#include <string>
#include <vector>

class AccessToken
{
	OT_DECL_NOCOPY(AccessToken)
	OT_DECL_DEFMOVE(AccessToken)
public:
	static AccessToken createNewAccessToken(const std::string& _userId, std::vector<uint8_t>&& _clientPublicKey, int64_t _expirationDate);

	static AccessToken readFromDatabase(mongocxx::client& _adminClient, const std::string& _token);

	void storeToDatabase(mongocxx::client& _adminClient) const;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	const std::string& getToken() const { return m_token; };
	const std::string& getUserId() const { return m_userId; };
	const std::vector<uint8_t>& getClientPublicKey() const { return m_clientPublicKey; };
	
	bool isExpired() const;
	int64_t getExpirationDate() const { return m_expirationDate; };
	
	bool isRevoked() const { return m_revocationDate != 0; };
	int64_t getRevocationDate() const { return m_revocationDate; };
	void revoke();

	// ###########################################################################################################################################################################################################################################################################################################################

private:
	std::string m_token;
	std::string m_userId;
	std::vector<uint8_t> m_clientPublicKey;
	int64_t m_expirationDate = 0;
	int64_t m_revocationDate = 0;
	
	// ###########################################################################################################################################################################################################################################################################################################################

	// Private

	AccessToken(const std::string& _token);
	AccessToken(std::string&& _token, const std::string& _userId, std::vector<uint8_t>&& _clientPublicKey, int64_t _expirationDate, int64_t _revocationDate = 0);
};