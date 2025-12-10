// @otlicense
// File: Base64Encoding.cpp
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

#include "Base64Encoding.h"
#include <Windows.h>

std::string encode(std::vector<unsigned char> _token)
{
    DWORD len = 0;
    // Get required buffer size
    CryptBinaryToStringA(_token.data(), static_cast<uint32_t>(_token.size()),
        CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
        NULL, &len);

    std::string out(len, '\0');

    CryptBinaryToStringA(_token.data(), static_cast<uint32_t>(_token.size()),
        CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
        &out[0], &len);
    return out;
}

std::vector<unsigned char> decode(const std::string& _token)
{
    DWORD decodedLen = 0;

    // First call: get required buffer size
    CryptStringToBinaryA(_token.c_str(), static_cast<uint32_t>(_token.size()),
        CRYPT_STRING_BASE64,
        NULL, &decodedLen,
        NULL, NULL);

    std::vector<BYTE> out(decodedLen);

    // Second call: decode into buffer
    CryptStringToBinaryA(_token.c_str(), static_cast<uint32_t>(_token.size()),
        CRYPT_STRING_BASE64,
        out.data(), &decodedLen,
        NULL, NULL);

    return out;
}

