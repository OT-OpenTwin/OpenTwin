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
        SECURITY_STATUS ss = AcquireCredentialsHandleW(
            NULL,
            (LPWSTR)L"Negotiate",
            SECPKG_CRED_OUTBOUND,
            NULL,
            NULL,
            NULL,
            NULL,
            &m_hCred,
            &m_lifetime);

        if (ss != SEC_E_OK)
        {
            std::cout << "AcquireCredentialsHandle failed\n";
        }
    }

    std::vector<BYTE> generateClientToken(
        const std::vector<BYTE>& inputToken,
        bool firstCall)
    {

        TimeStamp lpExpiry;
        ULONG ctxAttr = 0;

        // Input buffer

        SecBuffer inBuf;
        SecBufferDesc inDesc;
        if (!firstCall)
        {
            inBuf.BufferType = SECBUFFER_TOKEN;
            inBuf.cbBuffer = (ULONG)inputToken.size();
            inBuf.pvBuffer = (BYTE*)inputToken.data();

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

        SECURITY_STATUS ss = InitializeSecurityContextW(
            &m_hCred,
            m_pCtxHandle,
            (LPWSTR)L"",    // target name (SPN) – can be empty if NTLM/Negotiate
            ISC_REQ_CONFIDENTIALITY | ISC_REQ_MUTUAL_AUTH,
            0,
            SECURITY_NATIVE_DREP,
            firstCall ? NULL : &inDesc,
            0,
            &m_hCtx,
            &outDesc,
            &ctxAttr,
            &lpExpiry);

        if (ss != SEC_E_OK && ss != SEC_I_CONTINUE_NEEDED)
        {
            std::cout << "InitializeSecurityContext failed: " << std::hex << ss << "\n";
        }

        m_pCtxHandle = &m_hCtx;
        // Copy output token
        std::vector<BYTE> outputToken;
        outputToken.assign((BYTE*)outBuf.pvBuffer, (BYTE*)outBuf.pvBuffer + outBuf.cbBuffer);

        return outputToken;
    }

private:
    CredHandle m_hCred;
    TimeStamp m_lifetime;
    CtxtHandle m_hCtx;
    CtxtHandle* m_pCtxHandle = nullptr;
};


