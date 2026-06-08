#pragma once
#include "OTSystem/SingleSignOn/SingleSignOn_State.h"

namespace ot {
    class OT_SYS_API_EXPORT SingleSignOn_Client
    {
    public:
        SingleSignOn_Client(const std::wstring& _servicePrincipleName = L"");
        static std::wstring getActiveUserName();

        // If the token is an empty string, we generate the initial token.
		std::string generateToken(const std::string& _receivedToken);
        std::string getStateString() const { return m_state.getStateString(); }
        bool stateOK() const { return m_state.stateOK(); }
        bool stateContinueNeeded() const { return m_state.stateContinueNeeded(); }
    private:
        SingleSignOn_State m_state;
    };
}