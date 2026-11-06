// @otlicense

#pragma once

// OpenTwin header
#include "OTCrypto/Storage/CryptoKeyStore.h"

// std header
#include <filesystem>

namespace ot
{
	class OT_CRYPTO_API CryptoKeyStoreFileSystem : public CryptoKeyStore
	{
		OT_DECL_NODEFAULT(CryptoKeyStoreFileSystem)
		OT_DECL_NOCOPY(CryptoKeyStoreFileSystem)
		OT_DECL_NOMOVE(CryptoKeyStoreFileSystem)
	public:
		//! @brief Constructor.
		//! @param _keyStorePath The path to the key store directory.
		//! @param _createPath If true, the key store path will be created if it does not exist, if false, an exception will be thrown if the path does not exist.
		//! @throws ot::DirectoryNotFoundException if the key store path does not exist or could not be created.
		CryptoKeyStoreFileSystem(const std::filesystem::path& _keyStorePath, bool _createPath);

		virtual ~CryptoKeyStoreFileSystem() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Key storage

		//! @brief Checks if a key with the given key ID exists in the key store.
		virtual bool hasKey(const std::string& _keyId) const override;

		//! @brief Creates and stores a new key with the given key ID.
		//! @param _keyId The ID of the key to create.
		//! The key store may generate a unique ID if required by the underlaying
		//! implementation but must always try to use the requested key ID if possible.
		//! @return The ID of the created key (this may be different from the requested key ID).
		virtual std::string createKey(const std::string& _keyId) override;

		//! @brief Retrieves the public key associated with the given key ID.
		virtual std::vector<uint8_t> getPublicKey(const std::string& _keyId) const override;

		//! @brief Removes the key with the given key ID from the key store.
		//! @return True if the key was successfully removed, false if the key did not exist or could not be removed.
		virtual bool removeKey(const std::string& _keyId) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Signing

		virtual std::vector<uint8_t> sign(const std::string& _keyId, const std::vector<uint8_t>& _data) const override;

	private:
		std::filesystem::path keyToPath(const std::string& _keyName) const;
		std::filesystem::path keyToPublicKeyPath(const std::string& _keyName) const;
		std::filesystem::path keyToPrivateKeyPath(const std::string& _keyName) const;

		void writeKeyToFile(const std::filesystem::path& _filePath, const std::vector<uint8_t>& _keyData) const;

		std::filesystem::path m_keyStorePath;
	};
}