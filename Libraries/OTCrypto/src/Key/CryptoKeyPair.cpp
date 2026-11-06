// @otlicense

// OpenTwin header
#include "OTCrypto/Key/CryptoKeyPair.h"

ot::CryptoKeyPair::CryptoKeyPair(std::vector<uint8_t>&& _publicKey, std::vector<uint8_t>&& _privateKey)
	: m_publicKey(std::move(_publicKey)), m_privateKey(std::move(_privateKey))
{}
