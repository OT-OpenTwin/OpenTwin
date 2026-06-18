#pragma once
#include <optional>
#include <string>
#include <chrono>
namespace ot
{
	class AuthenticationProcessID
	{
	public:
		AuthenticationProcessID();
		AuthenticationProcessID(const std::string& _token);
		AuthenticationProcessID(const AuthenticationProcessID& _other) = delete;
		AuthenticationProcessID(AuthenticationProcessID&& _other) = default;
		AuthenticationProcessID& operator=(const AuthenticationProcessID& _other) = delete;
		AuthenticationProcessID& operator=(AuthenticationProcessID&& _other) = default;

		std::string getID() const { return m_id; }

		bool isIsExpired() const;

		bool operator==(const AuthenticationProcessID& other) const {
			return m_id == other.m_id;
		}

	private:

		std::string m_id;
		std::chrono::steady_clock::time_point m_creationTime;
		std::chrono::minutes m_idValidityDuration{ 5 };
	};
}

namespace std {
	template<>
	struct hash<ot::AuthenticationProcessID> {
		size_t operator()(const ot::AuthenticationProcessID& k) const 
		{
			return std::hash<std::string>{}(k.getID());
		}
	};
}
