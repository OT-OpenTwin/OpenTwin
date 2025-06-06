#pragma once
#include <mongocxx/client.hpp>

#include <string>

namespace DataStorageAPI
{
	class ThreadLocalClientInitialiser
	{
	public:
		void setCredentials(const std::string& _serverURL, const std::string& _userName, const std::string& _userPassword);

		mongocxx::client& getClient();
	private:
		std::string m_serverURL = "";
		std::string m_userName = "";
		std::string m_userPassword = "";

	};
}