#include "OTSystem/SingleSignOn/SingleSignOn_Server.h"
#include <cassert>

ot::SingleSignOn_Server::SingleSignOn_Server(const std::wstring& _servicePrincipleName)
    : m_state(SECPKG_CRED_INBOUND, _servicePrincipleName)
{
    if (!m_state.initializationSuccessful())
    {
        throw std::exception("AcquireCredentialsHandle inbound failed");
    }
    m_state.setFirstCall(false);
}

std::string ot::SingleSignOn_Server::processToken(std::string& _receivedEncodedToken)
{
    
    std::vector<unsigned char> receivedEncodedToken = SingleSignOn_State::decode(_receivedEncodedToken);
    std::vector<unsigned char> serverResponseToken;
     if (m_state.processToken(receivedEncodedToken, serverResponseToken))
     {
		 std::string encryptedToken = SingleSignOn_State::encode(serverResponseToken);
         std::vector<unsigned char> decryptedToken = SingleSignOn_State::decode(encryptedToken);
         assert(serverResponseToken == decryptedToken);
         return encryptedToken;
	 }
    return std::string();
}

void ot::SingleSignOn_Server::processLoggedInUserInfo()
{
    m_state.readUserName();
    m_state.readUserGroups();
	m_state.readPackageInfo();
}

