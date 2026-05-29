#pragma once
#include "OTSystem/SingleSignOn/SingleSignOn_State.h"

namespace ot {
    class OT_SYS_API_EXPORT SingleSignOn_Server
    {
    public:
        SingleSignOn_Server(const std::wstring& _servicePrincipleName = L"");

        // Potential improvement: General function to create a token and check the current state which may be completed or need to continue. The general approach would work with both kerberos and NTLM.
        std::string processToken(std::string& _receivedEncodedToken);
		void processLoggedInUserInfo();

    private:
        SingleSignOn_State m_state;
    };
}