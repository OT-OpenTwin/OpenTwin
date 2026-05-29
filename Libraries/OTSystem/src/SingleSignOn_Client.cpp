
#include "OTSystem/SingleSignOn/SingleSignOn_Client.h"
#include <windows.h>
#include <cassert>

using namespace ot;

SingleSignOn_Client::SingleSignOn_Client(const std::wstring& _servicePrincipleName)
    : m_state(SECPKG_CRED_OUTBOUND, _servicePrincipleName)
{
    if (!m_state.initializationSuccessful())
    {
        throw std::exception("AcquireCredentialsHandle failed");
    }
	m_state.setFirstCall(true);
}


std::wstring SingleSignOn_Client::getActiveUserName()
{
    TCHAR username[UNLEN + 1];
    DWORD size = UNLEN + 1;

    if (GetUserName(username, &size))
    {
        std::wstring out (username, size);
        return out;
    }
    else
    {
        throw std::exception("Failed to get the active user name.");
    }
}

std::string ot::SingleSignOn_Client::generateFirstToken()
{
    std::vector<unsigned char> temp;
    std::vector<unsigned char> token = m_state.generateToken(temp);
    const std::string encryptedToken = SingleSignOn_State::encode(token);
    std::vector<unsigned char> decryptedToken = SingleSignOn_State::decode(encryptedToken);
    assert(token == decryptedToken);
    return encryptedToken;
}

std::string ot::SingleSignOn_Client::generateThirdToken(std::string& _receivedToken)
{
	std::vector<unsigned char> receivedToken = SingleSignOn_State::decode(_receivedToken);
    std::vector<unsigned char> token = m_state.generateToken(receivedToken);
    const std::string encryptedToken = SingleSignOn_State::encode(token);
    std::vector<unsigned char> decryptedToken = SingleSignOn_State::decode(encryptedToken);
    assert(token == decryptedToken);
    return encryptedToken;
}
