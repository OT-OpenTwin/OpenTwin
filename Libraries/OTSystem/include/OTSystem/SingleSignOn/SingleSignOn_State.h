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

#pragma warning(disable:4251)

namespace ot {
    class OT_SYS_API_EXPORT SingleSignOn_State
    {
    public:
        SingleSignOn_State(unsigned long _fCredentialUse, const std::wstring& _servicePrincipleName = L"");
        SingleSignOn_State(const SingleSignOn_State& _other) = delete;
        SingleSignOn_State(SingleSignOn_State&& _other) = delete;
		SingleSignOn_State& operator=(const SingleSignOn_State& _other) = delete;
		SingleSignOn_State& operator=(SingleSignOn_State&& _other) = delete;
        ~SingleSignOn_State();
        bool initializationSuccessful() const;
		void setFirstCall(bool _firstCall) { m_firstCall = _firstCall; }

        std::vector<unsigned char> generateToken(std::vector<unsigned char>& _receivedEncodedToken);
        bool processToken(std::vector<unsigned char>& _receivedEncodedToken, std::vector<unsigned char>& _serverResponseToken);
        
        void readUserName();
        void readUserGroups();
        void readPackageInfo();

        static std::string encode(std::vector<unsigned char> _token);
        static std::vector<unsigned char> decode(const std::string& _token);
    private:
        bool m_firstCall = true;
        SECURITY_STATUS m_initialStatus;
        // service principle name (SPN) - can be empty if NTLM/Negotiate, needed for kerberos.
        std::wstring m_servicePrincipleName;
        CredHandle m_credHandle;
        TimeStamp m_credTimeStamp;
        
        CtxtHandle m_context;
        bool m_haveContext = false;
        
        std::vector<unsigned char> generateToken(BYTE* _inputToken, ULONG _tokenSize);
        SECURITY_STATUS acquireCredentialsHandle(CredHandle& _credHandle, TimeStamp& _credTimeStamp, unsigned long _fCredentialUse);
    };
}