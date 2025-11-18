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

class AuthorisationService
{
public:
    AuthorisationService()
        : m_hCtx{}
    {
        auto securityStatus  = acquireCredentialsHandle(m_credHandle, m_credTimeStamp, SECPKG_CRED_INBOUND);
        if (securityStatus != SEC_E_OK)
        {
            throw std::exception("AcquireCredentialsHandle inbound failed");
        }
    }

    std::string generateToken2(const std::string& _token1)
    {
        std::string base64EncodedToken = _token1;
        std::vector<BYTE> token = decode(base64EncodedToken);

        SecBuffer inBuf;
        inBuf.BufferType = SECBUFFER_TOKEN;
        inBuf.pvBuffer = token.data();
        inBuf.cbBuffer = static_cast<ULONG>(token.size());

        SecBufferDesc inDesc;
        inDesc.ulVersion = SECBUFFER_VERSION;
        inDesc.cBuffers = 1;
        inDesc.pBuffers = &inBuf;


        std::vector<BYTE> outBuffer(12288);
        SecBuffer outBuf;
        outBuf.BufferType = SECBUFFER_TOKEN;
        outBuf.cbBuffer = static_cast<ULONG>(outBuffer.size());
        outBuf.pvBuffer = outBuffer.data();

        SecBufferDesc outDesc;
        outDesc.ulVersion = SECBUFFER_VERSION;
        outDesc.cBuffers = 1;
        outDesc.pBuffers = &outBuf;

        unsigned long ctxAttr;


        SECURITY_STATUS securityStatus = AcceptSecurityContext(
            &m_credHandle,
            m_pCtxHandle,
            &inDesc,
            ASC_REQ_DELEGATE | ASC_REQ_CONFIDENTIALITY | ASC_REQ_MUTUAL_AUTH, SECURITY_NATIVE_DREP,
            &m_hCtx,
            &outDesc,
            &ctxAttr,
            &m_credTimeStamp);

        if (securityStatus != SEC_E_OK && securityStatus != SEC_I_CONTINUE_NEEDED)
        {
            std::cerr << "AcceptSecurityContext failed: " << securityStatus << "\n";
            return "";
        }

        assert(securityStatus == SEC_I_CONTINUE_NEEDED);

        m_pCtxHandle = &m_hCtx;

        const std::string token2 = encode(outBuffer);
        return token2;
    }
    void authorizeClient(const std::string& _token3)
    {
        std::string base64EncodedToken = _token3;
        std::vector<BYTE> token = decode(base64EncodedToken);

        SecBuffer inBuf;
        inBuf.BufferType = SECBUFFER_TOKEN;
        inBuf.pvBuffer = token.data();
        inBuf.cbBuffer = static_cast<ULONG>(token.size());

        SecBufferDesc inDesc;
        inDesc.ulVersion = SECBUFFER_VERSION;
        inDesc.cBuffers = 1;
        inDesc.pBuffers = &inBuf;


        std::vector<BYTE> outBuffer(12288);
        SecBuffer outBuf;
        outBuf.BufferType = SECBUFFER_TOKEN;
        outBuf.cbBuffer = static_cast<ULONG>(outBuffer.size());
        outBuf.pvBuffer = outBuffer.data();

        SecBufferDesc outDesc;
        outDesc.ulVersion = SECBUFFER_VERSION;
        outDesc.cBuffers = 1;
        outDesc.pBuffers = &outBuf;

        unsigned long ctxAttr;
        CtxtHandle hCtx;

        SECURITY_STATUS securityStatus = AcceptSecurityContext(
            &m_credHandle,
            m_pCtxHandle,
            &inDesc,
            ASC_REQ_DELEGATE | ASC_REQ_CONFIDENTIALITY | ASC_REQ_MUTUAL_AUTH, SECURITY_NATIVE_DREP,
            &hCtx,
            &outDesc,
            &ctxAttr,
            &m_credTimeStamp);

        if (securityStatus != SEC_E_OK && securityStatus != SEC_I_CONTINUE_NEEDED)
        {
            std::cerr << "AcceptSecurityContext failed: " << securityStatus << "\n";
            return;
        }

        if (securityStatus == SEC_E_OK)
        {
            readUserName(hCtx);
            readUserGroups(hCtx);
        }
    }

private:
    CredHandle m_credHandle;
    TimeStamp m_credTimeStamp;
    CtxtHandle m_hCtx;
    CtxtHandle* m_pCtxHandle = nullptr;

    void readUserName(CtxtHandle& _hCtx)
    {
        SecPkgContext_Names names;
        if (QueryContextAttributesW(&_hCtx, SECPKG_ATTR_NAMES, &names) == SEC_E_OK)
        {
            std::wcout << L"Authenticated user: " << names.sUserName << L"\n";
            FreeContextBuffer(names.sUserName);
        }
    }

    void readUserGroups(CtxtHandle _hCtx)
    {
        HANDLE hUserToken = NULL;
        SECURITY_STATUS ss = QuerySecurityContextToken(&_hCtx, &hUserToken);

        if (ss != SEC_E_OK) {
            std::cerr << "QuerySecurityContextToken failed\n";
            return;
        }

        DWORD needed = 0;
        GetTokenInformation(hUserToken, TokenGroups, NULL, 0, &needed);

        std::vector<BYTE> buffer(needed);
        PTOKEN_GROUPS groups = (PTOKEN_GROUPS)buffer.data();

        if (!GetTokenInformation(hUserToken, TokenGroups, groups, needed, &needed)) {
            std::cerr << "GetTokenInformation(TokenGroups) failed\n";
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
                std::wcout << L"Group: " << domain << L"\\" << name;

                // Attributes (enabled, owner, etc.)
                DWORD attrs = groups->Groups[i].Attributes;

                if (attrs & SE_GROUP_MANDATORY)     std::wcout << L" [MANDATORY]";
                if (attrs & SE_GROUP_ENABLED)       std::wcout << L" [ENABLED]";
                if (attrs & SE_GROUP_ENABLED_BY_DEFAULT) std::wcout << L" [DEFAULT]";

                std::wcout << L"\n";
            }
            else
            {
                std::wcout << L"Unknown SID\n";
            }
        }
    }
};

