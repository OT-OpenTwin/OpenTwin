#include "ClientLogIn.h"
#include "Base64Encoding.h"

ClientLogIn::ClientLogIn(const std::string& _targetName)
    : m_currentContext{}
{
    if (!_targetName.empty())
    {
        std::string target = "HOST/" + _targetName;
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, target.c_str(), -1, NULL, 0);
        m_buffer.resize(size_needed);
        MultiByteToWideChar(CP_UTF8, 0, target.c_str(), -1, m_buffer.data(), size_needed);
        m_targetName = m_buffer.data();
    }
    
    auto securityStatus = acquireCredentialsHandle(m_credHandle, m_credTimeStamp, SECPKG_CRED_OUTBOUND);
    if (securityStatus != SEC_E_OK)
    {
        throw std::exception("AcquireCredentialsHandle inbound failed");
    }
}

void ClientLogInAPI::execute(const std::string& _targetName)
{

    LPWSTR targetName = nullptr;
    std::vector<wchar_t> buffer;

    if (!_targetName.empty())
    {
        std::string target = "HOST/" + _targetName;
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, target.c_str(), -1, NULL, 0);
        buffer.resize(size_needed);
        MultiByteToWideChar(CP_UTF8, 0, target.c_str(), -1, buffer.data(), size_needed);
        targetName = buffer.data();
    }


    CtxtHandle hContext;
    CredHandle hCred;
    SecBuffer outBuffer;
    SecBufferDesc outDesc;
    DWORD ctxAttr;
    TimeStamp expiry;

    // Acquire outbound credentials for Negotiate
    AcquireCredentialsHandleW(
        NULL,
       (LPWSTR)L"Negotiate",                 // Let SSPI choose Kerberos or NTLM
        SECPKG_CRED_OUTBOUND,
        NULL,
        NULL,
        NULL,
        NULL,
        &hCred,
        &expiry
    );

    std::vector<BYTE> inToken;        // empty on first call
    std::vector<BYTE> outToken;

    bool continueNeeded = true;

    while (continueNeeded) {
        outToken.resize(16 * 1024); // allocate buffer
        outDesc.cBuffers = 1;
        outDesc.pBuffers = &outBuffer;
        outBuffer.BufferType = SECBUFFER_TOKEN;
        outBuffer.cbBuffer = static_cast<unsigned long>(outToken.size());
        outBuffer.pvBuffer = outToken.data();

        SecBuffer inSecBuffer;
        SecBufferDesc inDesc;
        SecBufferDesc* pInDesc = nullptr;
        if (!inToken.empty()) {
            inSecBuffer.BufferType = SECBUFFER_TOKEN;
            inSecBuffer.cbBuffer = static_cast<unsigned long>(inToken.size());
            inSecBuffer.pvBuffer = inToken.data();

            inDesc.cBuffers = 1;
            inDesc.pBuffers = &inSecBuffer;
            inDesc.ulVersion = SECBUFFER_VERSION;

            pInDesc = &inDesc;
        }

        SECURITY_STATUS status = InitializeSecurityContextW(
            &hCred,
            (hContext.dwLower || hContext.dwUpper) ? &hContext : NULL,
            targetName,  // SPN must match the registered SPN
            ISC_REQ_CONFIDENTIALITY | ISC_REQ_MUTUAL_AUTH,
            0,
            SECURITY_NATIVE_DREP,
            (inToken.empty() ? NULL : &inDesc),
            0,
            &hContext,
            &outDesc,
            &ctxAttr,
            &expiry
        );

        std::string encodedToken = encode(outToken);
        outToken = m_sendTokenFunc(encodedToken);

        if (status == SEC_E_OK) {
            continueNeeded = false;  // authentication finished
        }
        else if (status == SEC_I_CONTINUE_NEEDED) {
            // receive token from server into inToken and continue
        }
        else {
            // handle error
        }
    }
}

std::vector<unsigned char> ClientLogIn::generateClientToken(const std::vector<unsigned char>& _inputToken, bool _firstCall)
{

    TimeStamp lpExpiry;
    ULONG ctxAttr = 0;

    // Input buffer

    SecBuffer inBuf;
    SecBufferDesc inDesc;
    if (!_firstCall)
    {

        inBuf.BufferType = SECBUFFER_TOKEN;
        inBuf.cbBuffer = (ULONG)_inputToken.size();
        inBuf.pvBuffer = (BYTE*)_inputToken.data();

        inDesc.ulVersion = SECBUFFER_VERSION;
        inDesc.cBuffers = 1;
        inDesc.pBuffers = &inBuf;
    }
    else
    {
        assert(_inputToken.size() == 0);
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

    SECURITY_STATUS securityStatus = InitializeSecurityContextW(
        &m_credHandle,
        m_partialContext,
        m_targetName,    // target name (SPN) – can be empty if NTLM/Negotiate
        ISC_REQ_CONFIDENTIALITY | ISC_REQ_MUTUAL_AUTH,
        0,
        SECURITY_NATIVE_DREP,
        _firstCall ? NULL : &inDesc,
        0,
        &m_currentContext,
        &outDesc,
        &ctxAttr,
        &lpExpiry);

    if (securityStatus != SEC_E_OK && securityStatus != SEC_I_CONTINUE_NEEDED)
    {
        std::cout << "InitializeSecurityContext failed: " << std::hex << securityStatus << "\n";
    }


    m_partialContext = &m_currentContext;
    // Copy output token
    std::vector<BYTE> outputToken;
    outputToken.assign((BYTE*)outBuf.pvBuffer, (BYTE*)outBuf.pvBuffer + outBuf.cbBuffer);

    return outputToken;
}
