// @otlicense
// File: GlobalCryptoKeyStore.h
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
#include "OTCrypto/Storage/CryptoKeyStore.h"

// std header
#include <memory>

namespace ot
{
	//! @brief The GlobalCryptoKeyStore can be used as a global singleton for the CryptoKeyStore to be used.
	class OT_CRYPTO_API GlobalCryptoKeyStore
	{
		OT_DECL_NOCOPY(GlobalCryptoKeyStore)
		OT_DECL_NOMOVE(GlobalCryptoKeyStore)
	public:
		//! @brief Sets the key store used for signing and verification.
		//! @param _keyStore The key store to use. The GlobalCryptoKeyStore takes ownership of the provided CryptoKeyStore.
		//! @throws ot::ObjectAlreadyExistsException if a key store has already been set.
		static void setKeyStore(std::unique_ptr<CryptoKeyStore> _keyStore);

		//! @brief Returns the key store used for signing and verification.
		//! The caller takes ownership of the returned CryptoKeyStore.
		OT_DECL_NODISCARD static std::unique_ptr<CryptoKeyStore> takeKeyStore();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Key storage

		//! @brief Checks if a key with the given key ID exists in the key store.
		//! @throws ot::NotInitializedException if no key store has been set yet.
		OT_DECL_NODISCARD static bool hasKey(const std::string& _keyId);

		//! @brief Creates and stores a new key with the given key ID.
		//! @param _keyId The ID of the key to create.
		//! The key store may generate a unique ID if required by the underlaying
		//! implementation but must always try to use the requested key ID if possible.
		//! @throws ot::NotInitializedException if no key store has been set yet.
		//! @return The ID of the created key (this may be different from the requested key ID).
		OT_DECL_NODISCARD static std::string createKey(const std::string& _keyId);

		//! @brief Retrieves the public key associated with the given key ID.
		//! @throws ot::NotInitializedException if no key store has been set yet.
		OT_DECL_NODISCARD static std::vector<uint8_t> getPublicKey(const std::string& _keyId);

		//! @brief Removes the key with the given key ID from the key store.
		//! @throws ot::NotInitializedException if no key store has been set yet.
		//! @return True if the key was successfully removed, false if the key did not exist or could not be removed.
		OT_DECL_NODISCARD static bool removeKey(const std::string& _keyId);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Signing

		//! @brief Signs the given data using the key with the given key ID.
		//! @param _keyId The ID of the key to use for signing.
		//! @param _data The data to sign.
		//! @throws ot::NotInitializedException if no key store has been set yet.
		//! @return The signature of the data.
		OT_DECL_NODISCARD static std::vector<uint8_t> sign(const std::string& _keyId, const std::vector<uint8_t>& _data);

	private:
		static GlobalCryptoKeyStore& getInstance();

		//! @brief Returns the existing key store.
		//! The caller does not take ownership of the returned CryptoKeyStore.
		//! @throws ot::NotInitializedException if no key store has been set yet.
		static std::unique_ptr<CryptoKeyStore>& getExistingKeyStore();

		GlobalCryptoKeyStore();
		~GlobalCryptoKeyStore();

		std::unique_ptr<CryptoKeyStore> m_keyStore;
	};
}