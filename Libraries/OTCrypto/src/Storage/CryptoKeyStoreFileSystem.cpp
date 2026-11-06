// @otlicense

// OpenTwin header
#include "OTCrypto/Key/CryptoKeyGenerator.h"
#include "OTCrypto/Storage/CryptoKeyStoreFileSystem.h"
#include "OTCore/String.h"
#include "OTCore/Logging/Logger.h"

// std header
#include <fstream>

ot::CryptoKeyStoreFileSystem::CryptoKeyStoreFileSystem(const std::filesystem::path& _keyStorePath, bool _createPath)
	: m_keyStorePath(_keyStorePath)
{
	if (!std::filesystem::exists(m_keyStorePath))
	{
		if (_createPath)
		{
			if (!std::filesystem::create_directories(m_keyStorePath))
			{
				throw Exception::DirectoryNotFound("Failed to create key store path: " + m_keyStorePath.generic_string());
			}
		}
		else
		{
			throw Exception::DirectoryNotFound("Key store path does not exist: " + m_keyStorePath.generic_string());
		}
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Key storage

bool ot::CryptoKeyStoreFileSystem::hasKey(const std::string& _keyName) const
{
	return std::filesystem::exists(keyToPrivateKeyPath(_keyName)) 
		&& std::filesystem::exists(keyToPublicKeyPath(_keyName));
}

std::string ot::CryptoKeyStoreFileSystem::createKey(const std::string& _keyId)
{
	CryptoKeyPair keyPair = CryptoKeyGenerator::generateAsymmetricKeyPair();
	
	auto keyRootPath = keyToPath(_keyId);

	// Ensure key is not already stored
	if (std::filesystem::exists(keyRootPath))
	{
		throw Exception::ObjectAlreadyExists("Key with ID \"" + _keyId + "\" already exists in the key store.");
	}

	if (!std::filesystem::create_directories(keyRootPath))
	{
		throw Exception::FileOpen("Failed to create key directory for key ID: " + _keyId);
	}

	writeKeyToFile(keyToPublicKeyPath(_keyId), keyPair.getPublicKey());
	writeKeyToFile(keyToPrivateKeyPath(_keyId), keyPair.getPrivateKey());

	return _keyId;
}

std::vector<uint8_t> ot::CryptoKeyStoreFileSystem::getPublicKey(const std::string& _keyId) const
{
	std::ifstream publicKeyFile(keyToPublicKeyPath(_keyId), std::ios::binary);
	if (!publicKeyFile.is_open())
	{
		throw Exception::FileOpen("Failed to open public key file for key ID: " + _keyId);
	}

	std::vector<uint8_t> publicKeyData((std::istreambuf_iterator<char>(publicKeyFile)), std::istreambuf_iterator<char>());

	if (!publicKeyFile.good())
	{
		publicKeyFile.close();
		throw Exception::FileOpen("Failed to read public key data from file for key ID: " + _keyId);
	}
	else
	{
		publicKeyFile.close();
	}

	return publicKeyData;
}

bool ot::CryptoKeyStoreFileSystem::removeKey(const std::string& _keyId)
{
	if (!hasKey(_keyId))
	{
		return false;
	}

	try
	{
		std::filesystem::remove(keyToPublicKeyPath(_keyId));
		std::filesystem::remove(keyToPrivateKeyPath(_keyId));
		std::filesystem::remove(keyToPath(_keyId));
		return true;
	}
	catch (const std::filesystem::filesystem_error& e)
	{
		OT_LOG_E("Failed to remove key with ID \"" + _keyId + "\" from the key store: " + e.what());
		return false;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Signing

std::vector<uint8_t> ot::CryptoKeyStoreFileSystem::sign(const std::string& _keyId, const std::vector<uint8_t>& _data) const
{
	OT_LOG_E("Signing is not implemented for CryptoKeyStoreFileSystem. Key ID: " + _keyId);
	return std::vector<uint8_t>();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private methods

std::filesystem::path ot::CryptoKeyStoreFileSystem::keyToPath(const std::string& _keyName) const
{
	return m_keyStorePath / String::toHex(_keyName);
}

std::filesystem::path ot::CryptoKeyStoreFileSystem::keyToPublicKeyPath(const std::string& _keyName) const
{
	return keyToPath(_keyName) / "public.key";
}

std::filesystem::path ot::CryptoKeyStoreFileSystem::keyToPrivateKeyPath(const std::string& _keyName) const
{
	return keyToPath(_keyName) / "private.key";
}

void ot::CryptoKeyStoreFileSystem::writeKeyToFile(const std::filesystem::path& _filePath, const std::vector<uint8_t>& _keyData) const
{
	std::ofstream keyFile(_filePath, std::ios::binary);
	if (!keyFile.is_open())
	{
		throw Exception::FileOpen("Failed to open key file for writing");
	}

	keyFile.write(reinterpret_cast<const char*>(_keyData.data()), _keyData.size());

	if (!keyFile.good())
	{
		keyFile.close();
		throw Exception::FileOpen("Failed to write key data to file");
	}
	else
	{
		keyFile.close();
	}
}


