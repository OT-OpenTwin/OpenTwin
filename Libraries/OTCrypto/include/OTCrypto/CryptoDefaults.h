// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTCrypto/CryptoAPIExport.h"

namespace ot
{

    class OT_CRYPTO_API CryptoDefaults
    {
        OT_DECL_STATICONLY(CryptoDefaults)
    public:

        // Token Lenghts (bytes)

        static constexpr uint32_t accessTokenLength() { return 32; }

		// Ticket Lenghts (bytes)

        static constexpr uint32_t ticketIDLength() { return 16; }
		static constexpr uint32_t ticketLength() { return 32; }

		// User Session Lenghts (bytes)

        static constexpr uint32_t userSessionIDLength() { return 32; }
        static constexpr uint32_t userSessionTokenLength() { return 32; }

		// Challenge Lenghts (bytes)

        static constexpr uint32_t challengeLength() { return 32; }
		static constexpr uint32_t challengeIDLength() { return 16; }

    };

}