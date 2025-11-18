#pragma once
#include <string>
#include <Windows.h>
#include <vector>

std::string encode(std::vector<BYTE> _token)
{
    DWORD len = 0;
    // Get required buffer size
    CryptBinaryToStringA(_token.data(), _token.size(),
        CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
        NULL, &len);

    std::string out(len, '\0');

    CryptBinaryToStringA(_token.data(), (DWORD)_token.size(),
        CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
        &out[0], &len);
    return out;
}

std::vector<BYTE> decode(const std::string _token)
{
    DWORD decodedLen = 0;

    // First call: get required buffer size
    CryptStringToBinaryA(_token.c_str(), _token.size(),
        CRYPT_STRING_BASE64,
        NULL, &decodedLen,
        NULL, NULL);

    std::vector<BYTE> out(decodedLen);

    // Second call: decode into buffer
    CryptStringToBinaryA(_token.c_str(), _token.size(),
        CRYPT_STRING_BASE64,
        out.data(), &decodedLen,
        NULL, NULL);

    return out;
}

