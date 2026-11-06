// @otlicense

#pragma once

// OpenTwin header
#include "OTCrypto/Key/CryptoKeyPair.h"

namespace ot
{

	class OT_CRYPTO_API CryptoKeyGenerator
	{
		OT_DECL_STATICONLY(CryptoKeyGenerator)
	public:
		//! @brief Generates a new asymmetric key pair.
		//! @return A CryptoKeyPair object containing the generated public and private keys.
		OT_DECL_NODISCARD static CryptoKeyPair generateAsymmetricKeyPair();
	};

}