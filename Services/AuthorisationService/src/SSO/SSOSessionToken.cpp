// @otlicense

// Auth header
#include "SSO/SSOSessionToken.h"

// OpenTwin header
#include "OTSystem/RandomNumber/RandomNumberGeneratorCryptoSecure.h"
#include "OTCore/String.h"

std::string ot::SSOSessionToken::generateToken()
{
	RandomNumberGeneratorCryptoSecure generator;
	uint64_t tokenBase = generator.createPositiveRandomInt64();
	
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
