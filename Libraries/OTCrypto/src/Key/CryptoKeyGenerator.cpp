// @otlicense

// OpenTwin header
#include "OTCrypto/Key/CryptoKeyGenerator.h"

ot::CryptoKeyPair ot::CryptoKeyGenerator::generateAsymmetricKeyPair()
{
	std::string dummyPublicKey = "Dummy Public Key";
	std::string dummyPrivateKey = "Dummy Private Key";
	std::vector<uint8_t> publicKey(dummyPublicKey.begin(), dummyPublicKey.end());
	std::vector<uint8_t> privateKey(dummyPrivateKey.begin(), dummyPrivateKey.end());
	return CryptoKeyPair(std::move(publicKey), std::move(privateKey));
}