// @otlicense

// OpenTwin header
#include "OTCrypto/Storage/GlobalCryptoKeyStore.h"

void ot::GlobalCryptoKeyStore::setKeyStore(std::unique_ptr<CryptoKeyStore> _keyStore)
{
	if (getInstance().m_keyStore)
	{
		throw Exception::ObjectAlreadyExists("GlobalCryptoKeyStore already has a CryptoKeyStore set. Please take the existing key store before setting a new one.");
	}
	getInstance().m_keyStore = std::move(_keyStore);
}

std::unique_ptr<ot::CryptoKeyStore> ot::GlobalCryptoKeyStore::takeKeyStore()
{
	GlobalCryptoKeyStore& instance = getInstance();
	std::unique_ptr<CryptoKeyStore> keyStore = std::move(instance.m_keyStore);
	instance.m_keyStore.reset(nullptr);
	return std::move(keyStore);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Key storage

bool ot::GlobalCryptoKeyStore::hasKey(const std::string& _keyId)
{
	return getExistingKeyStore()->hasKey(_keyId);
}

std::string ot::GlobalCryptoKeyStore::createKey(const std::string& _keyId)
{
	return getExistingKeyStore()->createKey(_keyId);
}

std::vector<uint8_t> ot::GlobalCryptoKeyStore::getPublicKey(const std::string& _keyId)
{
	return getExistingKeyStore()->getPublicKey(_keyId);
}

bool ot::GlobalCryptoKeyStore::removeKey(const std::string& _keyId)
{
	return getExistingKeyStore()->removeKey(_keyId);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Signing

std::vector<uint8_t> ot::GlobalCryptoKeyStore::sign(const std::string& _keyId, const std::vector<uint8_t>& _data)
{
	return getExistingKeyStore()->sign(_keyId, _data);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private methods

ot::GlobalCryptoKeyStore& ot::GlobalCryptoKeyStore::getInstance()
{
	static GlobalCryptoKeyStore instance;
	return instance;
}

std::unique_ptr<ot::CryptoKeyStore>& ot::GlobalCryptoKeyStore::getExistingKeyStore()
{
	GlobalCryptoKeyStore& instance = getInstance();
	std::unique_ptr<CryptoKeyStore>& keyStore = instance.m_keyStore;

	if (!keyStore)
	{
		throw Exception::NotInitialized("GlobalCryptoKeyStore has not been initialized with a CryptoKeyStore implementation.");
	}

	return keyStore;
}

ot::GlobalCryptoKeyStore::GlobalCryptoKeyStore()
	: m_keyStore(nullptr)
{}

ot::GlobalCryptoKeyStore::~GlobalCryptoKeyStore()
{}
