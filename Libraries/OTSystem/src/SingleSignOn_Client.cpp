
#include "OTSystem/SingleSignOn/SingleSignOn_Client.h"
#include <windows.h>
#include <cassert>
#include <secext.h>   // GetUserNameExW

using namespace ot;

SingleSignOn_Client::SingleSignOn_Client(const std::wstring& _servicePrincipleName)
    : m_state(SECPKG_CRED_OUTBOUND, _servicePrincipleName)
{
    if (!m_state.stateOK())
    {
        throw std::exception("AcquireCredentialsHandle failed");
    }
	m_state.setFirstCall(true);
}


std::wstring SingleSignOn_Client::getActiveUserName()
{
    // NameSamCompatible → "DOMAIN\username"
    // NameUserPrincipal → "username@domain.com"  (Kerberos UPN)
    // NameDisplay       → "John Smith"           (friendly display name)

    DWORD size = 0;
    GetUserNameExW(NameSamCompatible, nullptr, &size); // first call gets required size

    std::wstring result(size, L'\0');
    if (GetUserNameExW(NameSamCompatible, result.data(), &size)) {
        result.resize(size); // trim the null terminator
        return result;
    }
    std::string error = "GetUserNameExW failed: " + std::to_string(GetLastError());
    throw std::exception(error.c_str());
}

std::string ot::SingleSignOn_Client::generateToken(const std::string& _receivedToken)
{
    std::vector<unsigned char> receivedToken;
    if (!_receivedToken.empty())
    {
	    receivedToken = SingleSignOn_State::decode(_receivedToken);
    }
    std::optional<std::vector<unsigned char>> token = m_state.generateToken(receivedToken);
    if (token.has_value())
    {
        const std::string encryptedToken = SingleSignOn_State::encode(token.value());
        std::vector<unsigned char> decryptedToken = SingleSignOn_State::decode(encryptedToken);
        assert(token == decryptedToken);
        return encryptedToken;
    }
    else
    {
        return "";
    }
}
