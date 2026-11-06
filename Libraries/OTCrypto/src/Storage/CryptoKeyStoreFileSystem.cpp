// @otlicense

// OpenTwin header
#include "OTCrypto/Key/CryptoKeyGenerator.h"
#include "OTCrypto/Signature/SignatureProvider.h"
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
	return getKey(_keyId, false);
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
	auto privateKey = getPrivateKey(_keyId);
	return SignatureProvider::sign(privateKey, _data);
}

bool ot::CryptoKeyStoreFileSystem::verify(const std::string& _keyId, const std::vector<uint8_t>& _data, const std::vector<uint8_t>& _signature) const
{
	auto publicKey = getPublicKey(_keyId);
	return SignatureProvider::verify(publicKey, _data, _signature);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private methods

std::vector<uint8_t> ot::CryptoKeyStoreFileSystem::getPrivateKey(const std::string& _keyId) const
{
	return getKey(_keyId, true);
}

std::vector<uint8_t> ot::CryptoKeyStoreFileSystem::getKey(const std::string& _keyId, bool _private) const
{
	std::filesystem::path keyPath = _private ? keyToPrivateKeyPath(_keyId) : keyToPublicKeyPath(_keyId);
	std::ifstream keyFile(keyPath, std::ios::binary);
	if (!keyFile.is_open())
	{
		throw Exception::FileOpen("Failed to open key file for key ID: " + _keyId);
	}

	std::vector<uint8_t> publicKeyData((std::istreambuf_iterator<char>(keyFile)), std::istreambuf_iterator<char>());

	if (!keyFile.good())
	{
		keyFile.close();
		throw Exception::FileOpen("Failed to read key data from file for key ID: " + _keyId);
	}
	else
	{
		keyFile.close();
	}

	return publicKeyData;
}

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


