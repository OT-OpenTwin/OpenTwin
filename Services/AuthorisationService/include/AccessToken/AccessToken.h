// @otlicense

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