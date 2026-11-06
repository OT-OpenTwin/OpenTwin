// @otlicense

// OpenTwin header
#include "OTCrypto/Signature/SignatureProvider.h"

// OpenSSL header
#include <openssl/evp.h>
#include <openssl/x509.h>

// std header
#include <memory>
#include <stdexcept>
#include <vector>
#include <cstdint>

// ###########################################################################################################################################################################################################################################################################################################################

// Signing

std::vector<uint8_t> ot::SignatureProvider::sign(const std::vector<uint8_t>& _privateKey, const std::vector<uint8_t>& _data)
{
    using EVPKeyPtr = std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)>;
    using EVPContextPtr = std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)>;

    // Deserialize private key (DER / PKCS#8)
    const unsigned char* buffer = _privateKey.data();

    EVPKeyPtr privateKey(
        d2i_AutoPrivateKey(
            nullptr,
            &buffer,
            static_cast<long>(_privateKey.size())),
        EVP_PKEY_free);

    if (!privateKey)
    {
        throw Exception::Runtime("Failed to deserialize private key.");
    }

    if (buffer != _privateKey.data() + _privateKey.size())
    {
        throw Exception::Runtime("Invalid private key data.");
    }

    if (EVP_PKEY_id(privateKey.get()) != EVP_PKEY_ED25519)
    {
        throw Exception::Runtime("Private key is not an Ed25519 key.");
    }

    // Create signing context
    EVPContextPtr context(EVP_MD_CTX_new(), EVP_MD_CTX_free);

    if (!context)
    {
        throw Exception::Runtime("Failed to create signing context.");
    }

    // Initialize Ed25519 signing
    if (EVP_DigestSignInit(
        context.get(),
        nullptr,
        nullptr,
        nullptr,
        privateKey.get()) != 1)
    {
        throw Exception::Runtime("Failed to initialize signing.");
    }

    // Determine signature size
    size_t signatureLength = 0;

    if (EVP_DigestSign(
        context.get(),
        nullptr,
        &signatureLength,
        _data.data(),
        _data.size()) != 1)
    {
        throw Exception::Runtime("Failed to determine signature size.");
    }

    // Generate signature
    std::vector<uint8_t> signature(signatureLength);

    if (EVP_DigestSign(
        context.get(),
        signature.data(),
        &signatureLength,
        _data.data(),
        _data.size()) != 1)
    {
        throw Exception::Runtime("Failed to sign data.");
    }

    signature.resize(signatureLength);

    return signature;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Verification

bool ot::SignatureProvider::verify(
    const std::vector<uint8_t>& _publicKey,
    const std::vector<uint8_t>& _data,
    const std::vector<uint8_t>& _signature)
{
    using EVPKeyPtr = std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)>;
    using EVPContextPtr = std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)>;

    // Deserialize public key (DER / SubjectPublicKeyInfo)
    const unsigned char* buffer = _publicKey.data();

    EVPKeyPtr publicKey(
        d2i_PUBKEY(
            nullptr,
            &buffer,
            static_cast<long>(_publicKey.size())),
        EVP_PKEY_free);

    if (!publicKey)
    {
        throw Exception::Runtime("Failed to deserialize public key.");
    }

    if (buffer != _publicKey.data() + _publicKey.size())
    {
        throw Exception::Runtime("Invalid public key data.");
    }

    if (EVP_PKEY_id(publicKey.get()) != EVP_PKEY_ED25519)
    {
        throw Exception::Runtime("Public key is not an Ed25519 key.");
    }

    // Create verification context
    EVPContextPtr context(EVP_MD_CTX_new(), EVP_MD_CTX_free);

    if (!context)
    {
        throw Exception::Runtime("Failed to create verification context.");
    }

    // Initialize Ed25519 verification
    if (EVP_DigestVerifyInit(
        context.get(),
        nullptr,
        nullptr,
        nullptr,
        publicKey.get()) != 1)
    {
        throw Exception::Runtime("Failed to initialize verification.");
    }

    // Verify signature
    int result = EVP_DigestVerify(
        context.get(),
        _signature.data(),
        _signature.size(),
        _data.data(),
        _data.size());

    if (result == 1)
    {
        return true;
    }

    if (result == 0)
    {
        return false;
    }

    throw Exception::Runtime("Internal signature verification error.");
}