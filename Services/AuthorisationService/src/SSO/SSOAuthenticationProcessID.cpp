// @otlicense

// Auth header
#include "SSO/SSOAuthenticationProcessID.h"

// OpenTwin header
#include "OTCore/String.h"
#include "OTSystem/RandomNumber/RandomNumberProvider.h"

ot::SSOAuthenticationProcessID::SSOAuthenticationProcessID()
{
	uint64_t tokenBase = RandomNumberProvider::createPositiveRandomInt64();

	auto* ptr = reinterpret_cast<const uint8_t*>(&tokenBase);
	std::vector<uint8_t> bytes(ptr, ptr + sizeof(tokenBase));
	m_id = ot::String::compressBase64(bytes.data(), bytes.size());

	m_creationTime = std::chrono::steady_clock::now();
}

ot::SSOAuthenticationProcessID::SSOAuthenticationProcessID(const std::string& _token)
{
	m_id = _token;
	m_creationTime = std::chrono::steady_clock::now();
}

bool ot::SSOAuthenticationProcessID::isIsExpired() const
{
	auto now = std::chrono::steady_clock::now();
	std::chrono::minutes passedTime = std::chrono::duration_cast<std::chrono::minutes>(now - m_creationTime);
	bool valid = passedTime < (m_idValidityDuration);
	return valid;
}
