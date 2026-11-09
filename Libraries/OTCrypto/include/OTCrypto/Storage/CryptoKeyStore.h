// @otlicense
// File: CryptoKeyStore.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// OpenTwin header
#include "OTCrypto/CryptoTypes.h"

namespace ot
{
	class OT_CRYPTO_API CryptoKeyStore
	{
		OT_DECL_NOCOPY(CryptoKeyStore)
		OT_DECL_DEFMOVE(CryptoKeyStore)
	public:
		CryptoKeyStore();
		virtual ~CryptoKeyStore() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Key storage

		//! @brief Checks if a key with the given key ID exists in the key store.
		virtual bool hasKey(const std::string& _keyId) const = 0;

		//! @brief Creates and stores a new key with the given key ID.
		//! @param _keyId The ID of the key to create.
		//! The key store may generate a unique ID if required by the underlaying
		//! implementation but must always try to use the requested key ID if possible.
		//! @return The ID of the created key (this may be different from the requested key ID).
		virtual std::string createKey(const std::string& _keyId) = 0;

		//! @brief Retrieves the public key associated with the given key ID.
		virtual std::vector<uint8_t> getPublicKey(const std::string& _keyId) const = 0;

		//! @brief Removes the key with the given key ID from the key store.
		//! @return True if the key was successfully removed, false if the key did not exist or could not be removed.
		virtual bool removeKey(const std::string& _keyId) = 0;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Signing

		//! @brief Signs the given data using the key with the given key ID.
		//! @param _keyId The ID of the key to use for signing.
		//! @param _data The data to sign.
		//! @return The signature of the data.
		virtual std::vector<uint8_t> sign(const std::string& _keyId, const std::vector<uint8_t>& _data) const = 0;

		//! @brief Verifies the given signature of the given data using the key with the given key ID.
		//! @param _keyId The ID of the key to use for verification.
		//! @param _data The data to verify.
		//! @param _signature The signature to verify.
		virtual bool verify(const std::string& _keyId, const std::vector<uint8_t>& _data, const std::vector<uint8_t>& _signature) const = 0;
	};
}