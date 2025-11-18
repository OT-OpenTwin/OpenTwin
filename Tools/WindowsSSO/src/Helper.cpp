#include "Helper.h"

std::string encode(std::vector<BYTE> _token)
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

std::vector<BYTE> decode(const std::string _token)
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

SECURITY_STATUS acquireCredentialsHandle(CredHandle& _credHandle, TimeStamp& _credTimeStamp, unsigned long _fCredentialUse)
{
    LPWSTR pPrincipal = NULL; //An entity recognized by the security system. Principals can include human users as well as autonomous processes.
    LPWSTR pPackage = const_cast<LPWSTR>(L"Negotiate"); //The software implementation of a security protocol. Options: Kerberos, NTLM, Schannel and CredSSP
    unsigned long fCredentialUse = _fCredentialUse; //A flag that indicates how these credentials will be used. SECPKG_CRED_OUTBOUND = Allow a local client credential to prepare an outgoing token. 
    void* pvLogonId = NULL; //This parameter is provided for file-system processes such as network redirectors.
    void* pAuthData = NULL; //Specifies authentication data for both Schannel and Negotiate packages
    SEC_GET_KEY_FN  pGetKeyFn = NULL;  //Reserved. This parameter is not used and should be set to NULL.
    void* pvGetKeyArgument = NULL; //Reserved. This parameter must be set to NULL.

    SECURITY_STATUS securityStatus = AcquireCredentialsHandleW(
        pPrincipal,
        pPackage,
        fCredentialUse,
        pvLogonId,
        pAuthData,
        pGetKeyFn,
        pvGetKeyArgument,
        &_credHandle,
        &_credTimeStamp);

    return securityStatus;
}

