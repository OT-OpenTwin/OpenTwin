#pragma once
#pragma warning(disable:4251)

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>

#include "OTSystem/ArchitectureInfo.h"
#include "OTSystem/SystemAPIExport.h"
#include "LogInInfos.h"


#if not defined (OT_OS_WINDOWS)
#error This is a windows only implementation
#endif
#define SECURITY_WIN32 1
#define WIN32_LEAN_AND_MEAN  // cut out unused subsystems
#define NOGDI                // fix the GetObject conflict
#define NOMINMAX             // protect std::min / std::max
#include <windows.h>
#include <wincrypt.h>       // CryptBinaryToString, CRYPT_STRING_NOCRLF, etc.
#include <Lmcons.h> 
#include <sspi.h>
#include <secext.h>  // GetUserNameExW

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
        bool stateOK() const;
        bool stateContinueNeeded() const;
        std::string getStateString() const;
		void setFirstCall(bool _firstCall) { m_firstCall = _firstCall; }

        std::optional<std::vector<unsigned char>> generateToken(std::vector<unsigned char>& _receivedEncodedToken);
        bool processToken(std::vector<unsigned char>& _receivedEncodedToken, std::vector<unsigned char>& _serverResponseToken);
        
        void readUserName(LogInInfos& _infos);
        void readUserGroups(LogInInfos& _infos);
        void readPackageInfo(LogInInfos& _infos);

        static std::string encode(std::vector<unsigned char> _token);
        static std::vector<unsigned char> decode(const std::string& _token);
    private:
        bool m_firstCall = true;
        SECURITY_STATUS m_currentStatus;
        // service principle name (SPN) - can be empty if NTLM/Negotiate, needed for kerberos.
        std::wstring m_servicePrincipleName;
        CredHandle m_credHandle;
        TimeStamp m_credTimeStamp;
        
        CtxtHandle m_context;
        bool m_haveContext = false;
        
        std::optional<std::vector<unsigned char>> generateToken(BYTE* _inputToken, ULONG _tokenSize);
        SECURITY_STATUS acquireCredentialsHandle(CredHandle& _credHandle, TimeStamp& _credTimeStamp, unsigned long _fCredentialUse);

        static const std::unordered_map<SECURITY_STATUS, std::string_view> m_securityStatusStrings;
		static const std::unordered_map < SID_NAME_USE, std::string_view> m_sidTypeStrings;
        static std::string secWcharToString(const SEC_WCHAR* _input);
   };
}