#pragma once
#include <string>
#include <vector>
#include "OTSystem/ArchitectureInfo.h"
#include "OTSystem/SystemAPIExport.h"
#if not defined (OT_OS_WINDOWS)
#error This is a windows only implementation
#endif
#define SECURITY_WIN32 1
#include <windows.h>
#include <Lmcons.h> 
#include <sspi.h>

namespace ot {
    class OT_SYS_API_EXPORT SingleSignOn_Client
    {
    public:
        SingleSignOn_Client(const std::wstring& _servicePrincipleName = L"");
        std::string generateToken(std::string& _receivedToken);
        std::wstring getActiveUserName();

    private:
        bool m_firstCall = true;
        // service principle name (SPN) - can be empty if NTLM/Negotiate, needed for kerberos.
        std::wstring m_servicePrincipleName;
        CredHandle m_credHandle;
        TimeStamp m_credTimeStamp;
        // On the first call to AcceptSecurityContext(CredSSP), this pointer receives the new context handle.
        // On subsequent calls, m_partialContext can be the same as this handle.
        CtxtHandle m_currentContext;
        // On the first call to AcceptSecurityContext (CredSSP), this pointer is NULL. 
        // On subsequent calls, phContext specifies the partially formed context returned in the phNewContext parameter by the first call.
        CtxtHandle* m_partialContext = nullptr;

        std::vector<unsigned char> generateTokenUnencoded(BYTE* _inputToken, ULONG _tokenSize);
        SECURITY_STATUS acquireCredentialsHandle(CredHandle& _credHandle, TimeStamp& _credTimeStamp, unsigned long _fCredentialUse);
    };
}