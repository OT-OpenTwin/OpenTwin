#include "Client.h"

Client::Client()
    : m_currentContext{}
{
    auto securityStatus = acquireCredentialsHandle(m_credHandle, m_credTimeStamp, SECPKG_CRED_OUTBOUND);
    if (securityStatus != SEC_E_OK)
    {
        throw std::exception("AcquireCredentialsHandle inbound failed");
    }
}

std::vector<BYTE> Client::generateClientToken(const std::vector<BYTE>& _inputToken, bool _firstCall)
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
        (LPWSTR)L"",    // target name (SPN) – can be empty if NTLM/Negotiate
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