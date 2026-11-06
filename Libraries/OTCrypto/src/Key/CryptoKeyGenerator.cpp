// @otlicense

// OpenTwin header
#include "OTCrypto/Key/CryptoKeyGenerator.h"

// OpenSSL header
#include <openssl/evp.h>
#include <openssl/x509.h>

// std header
#include <memory>
#include <vector>
#include <cstdint>

ot::CryptoKeyPair ot::CryptoKeyGenerator::generateAsymmetricKeyPair()
{
    using EVPContextPtr = std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)>;
    using EVPKeyPtr = std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)>;
    using PKCS8Ptr = std::unique_ptr<PKCS8_PRIV_KEY_INFO, decltype(&PKCS8_PRIV_KEY_INFO_free)>;

    // Create key generation context
    EVPContextPtr context(EVP_PKEY_CTX_new_id(EVP_PKEY_ED25519, nullptr), EVP_PKEY_CTX_free);

    if (!context)
    {
        throw Exception::Runtime("Failed to create key generation context.");
    }

    // Initialize key generation
    if (EVP_PKEY_keygen_init(context.get()) <= 0)
    {
        throw Exception::Runtime("Failed to initialize key generation.");
    }

    // Generate key pair
    EVP_PKEY* rawKey = nullptr;

    if (EVP_PKEY_keygen(context.get(), &rawKey) <= 0)
    {
        throw Exception::Runtime("Failed to generate asymmetric key pair.");
    }

    EVPKeyPtr key(rawKey, EVP_PKEY_free);

    // Serialize public key (DER / SubjectPublicKeyInfo)
    int publicKeyLength = i2d_PUBKEY(key.get(), nullptr);

    if (publicKeyLength <= 0)
    {
        throw Exception::Runtime("Failed to determine public key size.");
    }

    std::vector<uint8_t> publicKey(publicKeyLength);
    unsigned char* publicKeyBuffer = publicKey.data();

    if (i2d_PUBKEY(key.get(), &publicKeyBuffer) != publicKeyLength)
    {
        throw Exception::Runtime("Failed to serialize public key.");
    }

    // Convert private key to PKCS#8
    PKCS8Ptr privateKeyInfo(EVP_PKEY2PKCS8(key.get()), PKCS8_PRIV_KEY_INFO_free);

    if (!privateKeyInfo)
    {
        throw Exception::Runtime("Failed to convert private key to PKCS#8.");
    }

    // Serialize private key (DER / PKCS#8)
    int privateKeyLength = i2d_PKCS8_PRIV_KEY_INFO(privateKeyInfo.get(), nullptr);

    if (privateKeyLength <= 0)
    {
        throw Exception::Runtime("Failed to determine private key size.");
    }

    std::vector<uint8_t> privateKey(privateKeyLength);
    unsigned char* privateKeyBuffer = privateKey.data();

    if (i2d_PKCS8_PRIV_KEY_INFO(privateKeyInfo.get(), &privateKeyBuffer) != privateKeyLength)
    {
        throw Exception::Runtime("Failed to serialize private key.");
    }

    return CryptoKeyPair(std::move(publicKey), std::move(privateKey));
}