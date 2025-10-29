// @otlicense

#include "stdafx.h"
#include "ThreadLocalClientInitialiser.h"
#include <cassert>
#include "Connection/ConnectionAPI.h"

namespace DataStorageAPI
{
	thread_local static mongocxx::client g_client;
	thread_local static bool g_clientInitialised = false;
}

void DataStorageAPI::ThreadLocalClientInitialiser::setCredentials(const std::string& _serverURL, const std::string& _userName, const std::string& _userPassword)
{
	m_serverURL = _serverURL;
	m_userName = _userName;
	m_userPassword = _userPassword;
}

mongocxx::client& DataStorageAPI::ThreadLocalClientInitialiser::getClient()
{
	assert(!m_serverURL.empty() && !m_userName.empty() && !m_userPassword.empty());

	if (!g_clientInitialised)
	{
		std::string fixedServerURL(m_serverURL);
		if (m_serverURL.find("tls@") == 0)
		{
			fixedServerURL = fixedServerURL.substr(4);
		}

		std::string mongoServerURIString = DataStorageAPI::ConnectionAPI::getMongoURL(fixedServerURL, m_userName, m_userPassword);
		mongocxx::uri mongoServerURI(mongoServerURIString);

		g_client = mongocxx::client(mongoServerURI);
		g_clientInitialised = true;
	}
	return g_client;
}
