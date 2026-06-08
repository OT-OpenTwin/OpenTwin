#pragma once
#include "OTSystem/SingleSignOn/SingleSignOn_State.h"
#include "LogInInfos.h"
namespace ot {
    class OT_SYS_API_EXPORT SingleSignOn_Server
    {
    public:
        SingleSignOn_Server(const std::wstring& _servicePrincipleName = L"");
       
        // Potential improvement: General function to create a token and check the current state which may be completed or need to continue. The general approach would work with both kerberos and NTLM.
        std::string processToken(const std::string& _receivedEncodedToken);
        LogInInfos processLoggedInUserInfo();
        std::string getStateString() const { return m_state.getStateString(); }
        bool stateOK() const { return m_state.stateOK(); }
        bool stateContinueNeeded() const { return m_state.stateContinueNeeded(); }

    private:
        SingleSignOn_State m_state;
    };
}