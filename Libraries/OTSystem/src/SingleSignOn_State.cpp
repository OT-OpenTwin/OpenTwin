
#include "OTSystem/SingleSignOn/SingleSignOn_State.h"
#include <windows.h>
#include <cassert>

using namespace ot;

SingleSignOn_State::SingleSignOn_State(unsigned long _fCredentialUse, const std::wstring& _servicePrincipleName)
    : m_context{}, m_servicePrincipleName(_servicePrincipleName)
{
    m_initialStatus = acquireCredentialsHandle(m_credHandle, m_credTimeStamp, _fCredentialUse);
}


bool ot::SingleSignOn_State::initializationSuccessful() const
{
    return m_initialStatus == SEC_E_OK;
}

ot::SingleSignOn_State::~SingleSignOn_State()
{
    auto temp = 1;
}


std::vector<unsigned char> ot::SingleSignOn_State::generateToken(std::vector<unsigned char>& _receivedEncodedToken)
{
    size_t length = _receivedEncodedToken.size();
    BYTE* token = _receivedEncodedToken.size() == 0 ? nullptr : reinterpret_cast<BYTE*>(_receivedEncodedToken.data());

    std::vector<unsigned char> newToken = generateToken(token, static_cast<ULONG>(length));
    return newToken;
}


SECURITY_STATUS SingleSignOn_State::acquireCredentialsHandle(CredHandle& _credHandle, TimeStamp& _credTimeStamp, unsigned long _fCredentialUse)
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

std::vector<unsigned char> SingleSignOn_State::generateToken(BYTE* _inputToken, ULONG _tokenSize)
{
    TimeStamp lpExpiry;
    ULONG ctxAttr = 0;

    // Input buffer
    SecBuffer inBuf;
    SecBufferDesc inDesc;
    if (!m_firstCall)
    {
        assert(_tokenSize > 0);
        assert(_inputToken != nullptr);
        inBuf.BufferType = SECBUFFER_TOKEN;
        inBuf.cbBuffer = _tokenSize;
        inBuf.pvBuffer = _inputToken;

        inDesc.ulVersion = SECBUFFER_VERSION;
        inDesc.cBuffers = 1;
        inDesc.pBuffers = &inBuf;
    }


    // Output buffer
    BYTE outBufBytes[8192];
    SecBuffer outBuf;
    outBuf.BufferType = SECBUFFER_TOKEN;
    outBuf.cbBuffer = sizeof(outBufBytes);
    outBuf.pvBuffer = outBufBytes;

    SecBufferDesc outDesc;
    outDesc.ulVersion = SECBUFFER_VERSION;
    outDesc.cBuffers = 1;
    outDesc.pBuffers = &outBuf;

    LPWSTR targetName = m_servicePrincipleName.empty() ? nullptr : static_cast<LPWSTR>(m_servicePrincipleName.data());

    PCtxtHandle phContext = m_haveContext ? &m_context : nullptr;
    SECURITY_STATUS securityStatus = InitializeSecurityContextW(
        &m_credHandle,
        phContext,
        targetName,
        ISC_REQ_CONFIDENTIALITY | ISC_REQ_MUTUAL_AUTH,
        0,
        SECURITY_NATIVE_DREP,
        m_firstCall ? NULL : &inDesc,
        0,
        &m_context,
        &outDesc,
        &ctxAttr,
        &lpExpiry);


    if (securityStatus == SEC_I_CONTINUE_NEEDED || securityStatus == SEC_E_OK)
    {
        m_haveContext = true;
        m_firstCall = false;
        // Copy output token
        std::vector<BYTE> outputToken;
        outputToken.assign((BYTE*)outBuf.pvBuffer, (BYTE*)outBuf.pvBuffer + outBuf.cbBuffer);
        return outputToken;
    }
    else
    {
        throw std::exception(("InitializeSecurityContext failed: " + std::to_string(securityStatus)).c_str());
    }
}

std::string ot::SingleSignOn_State::encode(std::vector<unsigned char> _token)
{
    DWORD len = 0;

    // Get required buffer size (includes null terminator)
    CryptBinaryToStringA(_token.data(), static_cast<uint32_t>(_token.size()),
        CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
        NULL, &len);

    // Allocate string WITHOUT the null terminator
    std::string out;
    out.resize(len - 1);  // Subtract 1 for null terminator

    // Encode into the string
    DWORD actualLen = static_cast<DWORD>(out.size());
    CryptBinaryToStringA(_token.data(), static_cast<uint32_t>(_token.size()),
        CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF,
        &out[0], &actualLen);

    return out;
}

std::vector<unsigned char> ot::SingleSignOn_State::decode(const std::string& _token)
{
    DWORD decodedLen = 0;

    // First call: get required buffer size
    if (!CryptStringToBinaryA(_token.c_str(), static_cast<uint32_t>(_token.size()),
        CRYPT_STRING_BASE64,
        NULL, &decodedLen,
        NULL, NULL))
    {
        throw std::exception("CryptStringToBinaryA size query failed");
    }

    std::vector<BYTE> out(decodedLen);

    // Second call: decode into buffer
    DWORD actualLen = decodedLen;
    if (!CryptStringToBinaryA(_token.c_str(), static_cast<uint32_t>(_token.size()),
        CRYPT_STRING_BASE64,
        out.data(), &actualLen,
        NULL, NULL))
    {
        throw std::exception("CryptStringToBinaryA decoding failed");
    }

    // actualLen should equal decodedLen, but use it for safety
    out.resize(actualLen);
    return out;
}


bool ot::SingleSignOn_State::processToken(std::vector<unsigned char>& _receivedEncodedToken, std::vector<unsigned char>& _serverResponseToken)
{
    SecBuffer inBuf;
    inBuf.BufferType = SECBUFFER_TOKEN;
    inBuf.pvBuffer = _receivedEncodedToken.data();
    inBuf.cbBuffer = static_cast<ULONG>(_receivedEncodedToken.size());

    SecBufferDesc inDesc;
    inDesc.ulVersion = SECBUFFER_VERSION;
    inDesc.cBuffers = 1;
    inDesc.pBuffers = &inBuf;

    std::vector<BYTE> outBuffer(12288);
    SecBuffer outBuf;
    outBuf.BufferType = SECBUFFER_TOKEN;
    outBuf.cbBuffer = static_cast<ULONG>(outBuffer.size());  // Input: buffer size
    outBuf.pvBuffer = outBuffer.data();

    SecBufferDesc outDesc;
    outDesc.ulVersion = SECBUFFER_VERSION;
    outDesc.cBuffers = 1;
    outDesc.pBuffers = &outBuf;

    unsigned long ctxAttr = 0;

    PCtxtHandle phContext = m_haveContext ? &m_context : nullptr;
    SECURITY_STATUS securityStatus = AcceptSecurityContext(
        &m_credHandle,
        phContext,                    // NULL on first call, &m_completedContext on subsequent
        &inDesc,
        ASC_REQ_CONFIDENTIALITY | ASC_REQ_MUTUAL_AUTH,
        SECURITY_NATIVE_DREP,
        &m_context,
        &outDesc,
        &ctxAttr,
        &m_credTimeStamp);

    // ← Here outBuf.cbBuffer contains the ACTUAL token size
    // ← And outBuf.pvBuffer contains the actual token data (token2 on first call)

    // Save the server response token to send back to client
    _serverResponseToken.assign(
        static_cast<BYTE*>(outBuf.pvBuffer),
        static_cast<BYTE*>(outBuf.pvBuffer) + outBuf.cbBuffer);


    if (securityStatus == SEC_I_CONTINUE_NEEDED || securityStatus == SEC_E_OK)
    {
        m_haveContext = true;
        return true;
    }
    else
    {
        return false;
    }
    
}

void ot::SingleSignOn_State::readUserName()
{
    SecPkgContext_Names names;
    if (QueryContextAttributesW(&m_context, SECPKG_ATTR_NAMES, &names) == SEC_E_OK)
    {
        auto userName = names.sUserName;
        FreeContextBuffer(names.sUserName);
    }
}

void ot::SingleSignOn_State::readUserGroups()
{
    HANDLE hUserToken = NULL;
    SECURITY_STATUS ss = QuerySecurityContextToken(&m_context, &hUserToken);

    if (ss != SEC_E_OK) {
        throw std::exception("QuerySecurityContextToken failed");
        return;
    }

    DWORD needed = 0;
    GetTokenInformation(hUserToken, TokenGroups, NULL, 0, &needed);

    std::vector<BYTE> buffer(needed);
    PTOKEN_GROUPS groups = (PTOKEN_GROUPS)buffer.data();

    if (!GetTokenInformation(hUserToken, TokenGroups, groups, needed, &needed)) {
        throw std::exception("GetTokenInformation(TokenGroups) failed");
        return;
    }

    for (DWORD i = 0; i < groups->GroupCount; i++)
    {
        PSID sid = groups->Groups[i].Sid;

        WCHAR name[256];
        DWORD nameSize = 256;

        WCHAR domain[256];
        DWORD domainSize = 256;

        SID_NAME_USE use;

        if (LookupAccountSidW(
            NULL,  // local or domain
            sid,
            name, &nameSize,
            domain, &domainSize,
            &use))
        {
            auto group = domain;
            auto userName = name;

            // Attributes (enabled, owner, etc.)
            DWORD attrs = groups->Groups[i].Attributes;

            if (attrs & SE_GROUP_MANDATORY)
            {
                auto mandatory = L" [MANDATORY]";
            }
            if (attrs & SE_GROUP_ENABLED)
            {
                auto enabled = L" [ENABLED]";
            }
            if (attrs & SE_GROUP_ENABLED_BY_DEFAULT)
            {

                auto groupDefault = L" [DEFAULT]";
            }
        }
        else
        {
            auto state = L"Unknown SID";
        }
    }
}

void ot::SingleSignOn_State::readPackageInfo()
{

    SecPkgContext_PackageInfo pkgInfo;

    SECURITY_STATUS s = QueryContextAttributes(
        &m_context,
        SECPKG_ATTR_PACKAGE_INFO,
        &pkgInfo
    );

    if (s == SEC_E_OK)
    {
        auto packageName = pkgInfo.PackageInfo->Name;
        // Typically "Kerberos" or "NTLM"
        FreeContextBuffer(pkgInfo.PackageInfo);
    }
}



