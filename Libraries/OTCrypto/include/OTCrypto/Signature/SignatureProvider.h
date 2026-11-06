// @otlicense

#pragma once

// OpenTwin header
#include "OTCrypto/CryptoTypes.h"

namespace ot
{

	class OT_CRYPTO_API SignatureProvider
	{
		OT_DECL_STATICONLY(SignatureProvider)
	public:
		static std::vector<uint8_t> sign(const std::vector<uint8_t>& _privateKey, const std::vector<uint8_t>& _data);
		static bool verify(const std::vector<uint8_t>& _publicKey, const std::vector<uint8_t>& _data, const std::vector<uint8_t>& _signature);
	};

}