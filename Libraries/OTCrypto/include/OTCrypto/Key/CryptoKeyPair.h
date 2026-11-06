// @otlicense

#pragma once

// OpenTwin header
#include "OTCrypto/CryptoTypes.h"

namespace ot
{
	class OT_CRYPTO_API CryptoKeyPair
	{
		OT_DECL_NOCOPY(CryptoKeyPair)
		OT_DECL_DEFMOVE(CryptoKeyPair)
	public:
		explicit CryptoKeyPair() = default;
		explicit CryptoKeyPair(std::vector<uint8_t>&& _publicKey, std::vector<uint8_t>&& _privateKey);
		~CryptoKeyPair() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setPublicKey(std::vector<uint8_t>&& _publicKey) { m_publicKey = std::move(_publicKey); };
		const std::vector<uint8_t>& getPublicKey() const { return m_publicKey; };

		void setPrivateKey(std::vector<uint8_t>&& _privateKey) { m_privateKey = std::move(_privateKey); };
		const std::vector<uint8_t>& getPrivateKey() const { return m_privateKey; };

	private:
		std::vector<uint8_t> m_publicKey;
		std::vector<uint8_t> m_privateKey;

	};

}