#pragma once
#include <optional>
#include <string>
#include <chrono>
namespace ot
{
	class SessionToken
	{
	public:
		SessionToken() = default;
		SessionToken(const SessionToken& _other) = delete;
		SessionToken(SessionToken&& _other) = default;
		SessionToken& operator=(const SessionToken& _other) = delete;
		SessionToken& operator=(SessionToken&& _other) = default;
		
		std::string generateToken();
		std::optional<std::string> getToken();

	private:
		std::string m_token;
		std::chrono::steady_clock::time_point m_creationTime;
		std::chrono::minutes m_tokenValidityDurationAbsolute{ 30 }; 
		// std::chrono::minutes m_tokenValidityDurationIdle{ 30 }; Does not make sense since the authentication requests happen rarely
	};
}