// @otlicense
// File: AccessToken.cpp
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
#include "AccessToken/AccessToken.h"
#include "MongoDB/MongoConstants.h"
#include "MongoDB/MongoTokenFunctions.h"

// OpenTwin header
#include "OTSystem/DateTime.h"
#include "OTSystem/RandomNumber/RandomNumberProvider.h"
#include "OTCore/String.h"

// MongoDB header
#include <bsoncxx/builder/stream/document.hpp>

// ###########################################################################################################################################################################################################################################################################################################################

// Static methods

AccessToken AccessToken::createNewAccessToken(const std::string& _userId, std::vector<uint8_t>&& _clientPublicKey, int64_t _expirationDate)
{
	// Generate a random token
	std::vector<uint8_t> tokenData = ot::RandomNumberProvider::createRandomBytes(32);
	std::string tokenString = ot::String::toHexString(tokenData);

	return AccessToken(std::move(tokenString), _userId, std::move(_clientPublicKey), _expirationDate);
}

AccessToken AccessToken::readFromDatabase(mongocxx::client& _adminClient, const std::string& _token)
{
	AccessToken token(_token);

	mongocxx::database secondaryDb = _adminClient.database(MongoConstants::USER_DB);
	mongocxx::collection sessionsCollection = secondaryDb.collection(MongoConstants::ACCESS_TOKENS_COLLECTION);

	bsoncxx::document::value find_session_query = bsoncxx::builder::stream::document{}
		<< "token" << token.getToken()
		<< bsoncxx::builder::stream::finalize;

	auto tokenDoc = sessionsCollection.find_one(find_session_query.view());

	if (!tokenDoc)
	{
		throw ot::Exception::ObjectNotFound("Token not found in database");
	}

	const auto& view = tokenDoc->view();
	
	OTAssert(view["token"].get_utf8().value == token.getToken(), "Token mismatch in database");

	token.m_userId = view["user_id"].get_utf8().value;
	token.m_expirationDate = view["expiration_date"].get_int64();
	token.m_revocationDate = view["revocation_date"].get_int64();

	std::string clientKey64(view["client_public_key"].get_utf8().value);
	token.m_clientPublicKey = ot::String::bytesFromBase64Url(clientKey64);

	return token;
}

void AccessToken::storeToDatabase(mongocxx::client& _adminClient) const
{
	mongocxx::database secondaryDb = _adminClient.database(MongoConstants::USER_DB);
	mongocxx::collection tokensCollection = secondaryDb.collection(MongoConstants::ACCESS_TOKENS_COLLECTION);

	bsoncxx::document::value session_value = bsoncxx::builder::stream::document{}
		<< "token" << m_token
		<< "user_id" << m_userId
		<< "client_public_key" << ot::String::toBase64Url(m_clientPublicKey)
		<< "expiration_date" << m_expirationDate
		<< "revocation_date" << m_revocationDate
		<< bsoncxx::builder::stream::finalize;
	
	tokensCollection.insert_one(session_value.view());
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter

bool AccessToken::isExpired() const
{
	return ot::DateTime::msSinceEpoch() > m_expirationDate;
}

void AccessToken::revoke()
{
	m_revocationDate = ot::DateTime::msSinceEpoch();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Constructor

AccessToken::AccessToken(const std::string& _token)
	: m_token(_token)
{}

AccessToken::AccessToken(std::string&& _token, const std::string& _userId, std::vector<uint8_t>&& _clientPublicKey, int64_t _expirationDate, int64_t _revocationDate)
	: m_token(std::move(_token)), m_userId(_userId), m_clientPublicKey(std::move(_clientPublicKey)), m_expirationDate(_expirationDate), m_revocationDate(_revocationDate)
{}