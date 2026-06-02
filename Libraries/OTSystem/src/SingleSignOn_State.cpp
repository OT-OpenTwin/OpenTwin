
#include "OTSystem/SingleSignOn/SingleSignOn_State.h"
#include <cassert>

using namespace ot;

const std::unordered_map<SECURITY_STATUS, std::string_view> SingleSignOn_State::m_securityStatusStrings = {
    // Success / Informational
    { SEC_E_OK,                          "SEC_E_OK: Operation completed successfully" },
    { SEC_I_CONTINUE_NEEDED,             "SEC_I_CONTINUE_NEEDED: Token exchange not yet complete" },
    { SEC_I_COMPLETE_NEEDED,             "SEC_I_COMPLETE_NEEDED: CompleteToken must be called" },
    { SEC_I_COMPLETE_AND_CONTINUE,       "SEC_I_COMPLETE_AND_CONTINUE: CompleteToken + continue needed" },
    { SEC_I_LOCAL_LOGON,                 "SEC_I_LOCAL_LOGON: Local logon occurred" },
    { SEC_I_CONTEXT_EXPIRED,             "SEC_I_CONTEXT_EXPIRED: Security context expired" },
    { SEC_I_INCOMPLETE_CREDENTIALS,      "SEC_I_INCOMPLETE_CREDENTIALS: Credentials incomplete" },
    { SEC_I_RENEGOTIATE,                 "SEC_I_RENEGOTIATE: Context renegotiation required" },

    // Errors
    { SEC_E_INSUFFICIENT_MEMORY,         "SEC_E_INSUFFICIENT_MEMORY: Not enough memory" },
    { SEC_E_INVALID_HANDLE,              "SEC_E_INVALID_HANDLE: Invalid handle" },
    { SEC_E_UNSUPPORTED_FUNCTION,        "SEC_E_UNSUPPORTED_FUNCTION: Function not supported" },
    { SEC_E_TARGET_UNKNOWN,              "SEC_E_TARGET_UNKNOWN: Target unknown" },
    { SEC_E_INTERNAL_ERROR,              "SEC_E_INTERNAL_ERROR: Internal error" },
    { SEC_E_SECPKG_NOT_FOUND,            "SEC_E_SECPKG_NOT_FOUND: Security package not found" },
    { SEC_E_NOT_OWNER,                   "SEC_E_NOT_OWNER: Caller is not owner" },
    { SEC_E_CANNOT_INSTALL,              "SEC_E_CANNOT_INSTALL: Package cannot be installed" },
    { SEC_E_INVALID_TOKEN,               "SEC_E_INVALID_TOKEN: Token is invalid" },
    { SEC_E_CANNOT_PACK,                 "SEC_E_CANNOT_PACK: Cannot pack token" },
    { SEC_E_QOP_NOT_SUPPORTED,           "SEC_E_QOP_NOT_SUPPORTED: Quality of protection not supported" },
    { SEC_E_NO_IMPERSONATION,            "SEC_E_NO_IMPERSONATION: No impersonation possible" },
    { SEC_E_LOGON_DENIED,                "SEC_E_LOGON_DENIED: Logon denied (bad credentials)" },
    { SEC_E_UNKNOWN_CREDENTIALS,         "SEC_E_UNKNOWN_CREDENTIALS: Unknown credentials" },
    { SEC_E_NO_CREDENTIALS,              "SEC_E_NO_CREDENTIALS: No credentials available" },
    { SEC_E_MESSAGE_ALTERED,             "SEC_E_MESSAGE_ALTERED: Message has been altered" },
    { SEC_E_OUT_OF_SEQUENCE,             "SEC_E_OUT_OF_SEQUENCE: Message out of sequence" },
    { SEC_E_NO_AUTHENTICATING_AUTHORITY, "SEC_E_NO_AUTHENTICATING_AUTHORITY: No authenticating authority" },
    { SEC_E_BAD_PKGID,                   "SEC_E_BAD_PKGID: Bad package ID" },
    { SEC_E_CONTEXT_EXPIRED,             "SEC_E_CONTEXT_EXPIRED: Context expired" },
    { SEC_E_INCOMPLETE_MESSAGE,          "SEC_E_INCOMPLETE_MESSAGE: Message is incomplete" },
    { SEC_E_BUFFER_TOO_SMALL,            "SEC_E_BUFFER_TOO_SMALL: Buffer too small" },
    { SEC_E_WRONG_PRINCIPAL,             "SEC_E_WRONG_PRINCIPAL: Wrong principal (Kerberos SPN mismatch)" },
    { SEC_E_TIME_SKEW,                   "SEC_E_TIME_SKEW: Clock skew too large (Kerberos)" },
    { SEC_E_UNTRUSTED_ROOT,              "SEC_E_UNTRUSTED_ROOT: Certificate chain untrusted root" },
    { SEC_E_ILLEGAL_MESSAGE,             "SEC_E_ILLEGAL_MESSAGE: Illegal message received" },
    { SEC_E_CERT_UNKNOWN,                "SEC_E_CERT_UNKNOWN: Certificate unknown" },
    { SEC_E_CERT_EXPIRED,                "SEC_E_CERT_EXPIRED: Certificate expired" },
    { SEC_E_ENCRYPT_FAILURE,             "SEC_E_ENCRYPT_FAILURE: Encryption failure" },
    { SEC_E_DECRYPT_FAILURE,             "SEC_E_DECRYPT_FAILURE: Decryption failure" },
    { SEC_E_ALGORITHM_MISMATCH,          "SEC_E_ALGORITHM_MISMATCH: Algorithm mismatch" },
    { SEC_E_MUTUAL_AUTH_FAILED,          "SEC_E_MUTUAL_AUTH_FAILED: Mutual authentication failed" },
    { SEC_E_SHUTDOWN_IN_PROGRESS,        "SEC_E_SHUTDOWN_IN_PROGRESS: Shutdown in progress" },
};
const std::unordered_map<SID_NAME_USE, std::string_view> SingleSignOn_State::m_sidTypeStrings = {
    { SidTypeUser, "User" },
    { SidTypeGroup, "Group" },
    { SidTypeDomain, "Domain" },
    { SidTypeAlias, "Alias" },
    { SidTypeWellKnownGroup, "Well-known group" },
    { SidTypeDeletedAccount, "Deleted account" },
    { SidTypeInvalid, "Invalid" },
    { SidTypeUnknown, "Unknown" }
};

SingleSignOn_State::SingleSignOn_State(unsigned long _fCredentialUse, const std::wstring& _servicePrincipleName)
    : m_context{}, m_servicePrincipleName(_servicePrincipleName)
{
    m_currentStatus = acquireCredentialsHandle(m_credHandle, m_credTimeStamp, _fCredentialUse);
}


ot::SingleSignOn_State::~SingleSignOn_State()
{
    if (m_haveContext) {
        DeleteSecurityContext(&m_context);
        m_haveContext = false;
    }

    if (SecIsValidHandle(&m_credHandle)) {
        FreeCredentialsHandle(&m_credHandle);
        SecInvalidateHandle(&m_credHandle);
    }
}

bool ot::SingleSignOn_State::stateOK() const
{
    return m_currentStatus == SEC_E_OK;
}

bool ot::SingleSignOn_State::stateContinueNeeded() const
{
    return m_currentStatus == SEC_I_CONTINUE_NEEDED;
}

std::string ot::SingleSignOn_State::getStateString() const
{
    auto statusString = m_securityStatusStrings.find(m_currentStatus);
    if (statusString != m_securityStatusStrings.end())
    {
        return statusString->second.data();
    }
    else
    {
		return "Unresolved error code: " + std::to_string(m_currentStatus);
    }
}


std::optional<std::vector<unsigned char>> ot::SingleSignOn_State::generateToken(std::vector<unsigned char>& _receivedEncodedToken)
{
    size_t length = _receivedEncodedToken.size();
    BYTE* token = _receivedEncodedToken.size() == 0 ? nullptr : reinterpret_cast<BYTE*>(_receivedEncodedToken.data());

    std::optional<std::vector<unsigned char>> newToken = generateToken(token, static_cast<ULONG>(length));
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

std::string ot::SingleSignOn_State::secWcharToString(const SEC_WCHAR* _input)
{
    if (!_input) 
    {
        return {};
    }

    int size = WideCharToMultiByte(CP_UTF8, 0, _input, -1, nullptr, 0, nullptr, nullptr);
    if (size <= 0)
    {
        return {};
    }

    std::string result(size - 1, '\0'); // -1 to exclude null terminator
    WideCharToMultiByte(CP_UTF8, 0, _input, -1, result.data(), size, nullptr, nullptr);
    return result;
}

std::optional<std::vector<unsigned char>> SingleSignOn_State::generateToken(BYTE* _inputToken, ULONG _tokenSize)
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
    m_currentStatus = InitializeSecurityContextW(
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


    if (m_currentStatus == SEC_I_CONTINUE_NEEDED || m_currentStatus == SEC_E_OK)
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
        return std::nullopt;
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
    m_currentStatus = AcceptSecurityContext(
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


    if (m_currentStatus == SEC_I_CONTINUE_NEEDED || m_currentStatus == SEC_E_OK)
    {
        m_haveContext = true;
        return true;
    }
    else
    {
        return false;
    }
    
}

void ot::SingleSignOn_State::readUserName(LogInInfos& _infos)
{
    SecPkgContext_Names names;
    if (QueryContextAttributesW(&m_context, SECPKG_ATTR_NAMES, &names) == SEC_E_OK)
    {
        
        auto userName = names.sUserName;
        _infos.m_userName = secWcharToString(userName);
        FreeContextBuffer(names.sUserName);
    }
}

void ot::SingleSignOn_State::readUserGroups(LogInInfos& _infos)
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
        GroupInfo groupInfo;

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
            
            auto tempName = std::wstring(name, nameSize);
            groupInfo.m_name = secWcharToString(name);
            auto tempDomain = std::wstring(domain, domainSize);
			groupInfo.m_domain = secWcharToString(domain);
            
            auto useType =  m_sidTypeStrings.find(use);
            if(useType != m_sidTypeStrings.end())
            {
                groupInfo.m_sidType = useType->second.data();
			}
            else
            {
				groupInfo.m_sidType = "Unknown SID";
            }
            groupInfo.m_mandatory = (attrs & SE_GROUP_MANDATORY) != 0;
            groupInfo.m_enabled = (attrs & SE_GROUP_ENABLED) != 0;
            groupInfo.m_byDefault = (attrs & SE_GROUP_ENABLED_BY_DEFAULT) != 0;
            groupInfo.m_owner = (attrs & SE_GROUP_OWNER) != 0;
            groupInfo.m_logonId = (attrs & SE_GROUP_LOGON_ID) != 0;
            _infos.m_userGroups.push_back(groupInfo);
        }
        else
        {
            auto state = L"Unknown SID";
        }
    }
}

void ot::SingleSignOn_State::readPackageInfo(LogInInfos& _infos)
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
		_infos.m_packageName = secWcharToString(packageName);
        // Typically "Kerberos" or "NTLM"
        FreeContextBuffer(pkgInfo.PackageInfo);
    }
}



