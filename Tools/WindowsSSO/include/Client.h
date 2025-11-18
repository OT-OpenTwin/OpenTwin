#pragma once
#define SECURITY_WIN32 1
#include <windows.h>
#include <sspi.h>
#include <strsafe.h>
#include <iostream>
#include <vector>
#include <string>
#include <winhttp.h>
#pragma comment(lib, "secur32.lib")
#pragma comment(lib, "winhttp.lib")
#include <cassert>
#include "Helper.h"

class Client
{
public:
    Client()
        : m_hCtx{}
    {
        auto securityStatus = acquireCredentialsHandle(m_credHandle, m_credTimeStamp, SECPKG_CRED_OUTBOUND);
        if (securityStatus != SEC_E_OK)
        {
            throw std::exception("AcquireCredentialsHandle inbound failed");
        }
    }

    std::vector<BYTE> generateClientToken(const std::vector<BYTE>& _inputToken, bool _firstCall)
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
            m_pCtxHandle,
            (LPWSTR)L"",    // target name (SPN) – can be empty if NTLM/Negotiate
            ISC_REQ_CONFIDENTIALITY | ISC_REQ_MUTUAL_AUTH,
            0,
            SECURITY_NATIVE_DREP,
            _firstCall ? NULL : &inDesc,
            0,
            &m_hCtx,
            &outDesc,
            &ctxAttr,
            &lpExpiry);

        if (securityStatus != SEC_E_OK && securityStatus != SEC_I_CONTINUE_NEEDED)
        {
            std::cout << "InitializeSecurityContext failed: " << std::hex << securityStatus << "\n";
        }
        
        
        m_pCtxHandle = &m_hCtx;
        // Copy output token
        std::vector<BYTE> outputToken;
        outputToken.assign((BYTE*)outBuf.pvBuffer, (BYTE*)outBuf.pvBuffer + outBuf.cbBuffer);

        return outputToken;
    }

private:
    CredHandle m_credHandle;
    TimeStamp m_credTimeStamp;
    CtxtHandle m_hCtx;
    CtxtHandle* m_pCtxHandle = nullptr;
};


