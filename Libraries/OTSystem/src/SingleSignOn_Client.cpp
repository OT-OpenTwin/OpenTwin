
#include "OTSystem/SingleSignOn/SingleSignOn_Client.h"
#include <windows.h>
#include <cassert>

using namespace ot;

SingleSignOn_Client::SingleSignOn_Client(const std::wstring& _servicePrincipleName)
    : m_currentContext{}, m_servicePrincipleName(_servicePrincipleName)
{
    auto securityStatus = acquireCredentialsHandle(m_credHandle, m_credTimeStamp, SECPKG_CRED_OUTBOUND);
    if (securityStatus != SEC_E_OK)
    {
        throw std::exception("AcquireCredentialsHandle inbound failed");
    }
}

std::string SingleSignOn_Client::generateToken(std::string& _receivedToken)
{
    size_t length = _receivedToken.size();
    BYTE* token = _receivedToken.size() == 0 ? nullptr : reinterpret_cast<BYTE*>(_receivedToken.data());

    std::vector<unsigned char> newToken = generateTokenUnencoded(token, static_cast<ULONG>(length));
	
	//Convert to string for easier handling
    std::string out(newToken.begin(), newToken.end());
    if(out[newToken.size() - 1] != '\0')
    {
        out.push_back('\0');
	}

    return out;
}

std::wstring SingleSignOn_Client::getActiveUserName()
{
    TCHAR username[UNLEN + 1];
    DWORD size = UNLEN + 1;

    if (GetUserName(username, &size))
    {
        std::wstring out (username, size);
        return out;
    }
    else
    {
        throw std::exception("Failed to get the active user name.");
    }
}

std::vector<unsigned char> SingleSignOn_Client::generateTokenUnencoded(BYTE* _inputToken, ULONG _tokenSize)
{
    TimeStamp lpExpiry;
    ULONG ctxAttr = 0;

    // Input buffer
    SecBuffer inBuf;
    SecBufferDesc inDesc;
    if (m_firstCall)
    {
        assert(_tokenSize == 0);
		assert(_inputToken == nullptr);
		m_firstCall = false;
    }
    else
    {
        assert(_tokenSize > 0);
        assert(_inputToken != nullptr);
        inBuf.BufferType = SECBUFFER_TOKEN;
        inBuf.cbBuffer = _tokenSize;
        inBuf.pvBuffer = &_inputToken;

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

    SECURITY_STATUS securityStatus = InitializeSecurityContextW(
        &m_credHandle,
        m_partialContext,
        targetName,
        ISC_REQ_CONFIDENTIALITY | ISC_REQ_MUTUAL_AUTH,
        0,
        SECURITY_NATIVE_DREP,
        m_firstCall ? NULL : &inDesc,
        0,
        &m_currentContext,
        &outDesc,
        &ctxAttr,
        &lpExpiry);

    if (securityStatus != SEC_E_OK && securityStatus != SEC_I_CONTINUE_NEEDED)
    {
        throw std::exception("InitializeSecurityContext failed: " + securityStatus);
    }


    m_partialContext = &m_currentContext;
    // Copy output token
    std::vector<BYTE> outputToken;
    outputToken.assign((BYTE*)outBuf.pvBuffer, (BYTE*)outBuf.pvBuffer + outBuf.cbBuffer);

    return outputToken;
}

SECURITY_STATUS SingleSignOn_Client::acquireCredentialsHandle(CredHandle& _credHandle, TimeStamp& _credTimeStamp, unsigned long _fCredentialUse)
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

