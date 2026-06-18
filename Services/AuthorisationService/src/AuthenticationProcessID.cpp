#include "AuthenticationProcessID.h"
#include "OTSystem/RandomNumber/RandomNumberGeneratorCryptoSecure.h"
#include "OTCore/String.h"

ot::AuthenticationProcessID::AuthenticationProcessID()
{
	RandomNumberGeneratorCryptoSecure generator;
	uint64_t tokenBase = generator.createPositiveRandomInt64();

	auto* ptr = reinterpret_cast<const uint8_t*>(&tokenBase);
	std::vector<uint8_t> bytes(ptr, ptr + sizeof(tokenBase));
	m_id = ot::String::compressBase64(bytes.data(), bytes.size());

	m_creationTime = std::chrono::steady_clock::now();
}

ot::AuthenticationProcessID::AuthenticationProcessID(const std::string& _token)
{
	m_id= _token;
	m_creationTime = std::chrono::steady_clock::now();
}


bool ot::AuthenticationProcessID::isIsExpired() const
{
	auto now = std::chrono::steady_clock::now();
	std::chrono::minutes passedTime = std::chrono::duration_cast<std::chrono::minutes>(now - m_creationTime);
	bool valid = passedTime < (m_idValidityDuration);
	return valid;
}
