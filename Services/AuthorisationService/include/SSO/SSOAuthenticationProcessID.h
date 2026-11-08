// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"

// std header
#include <chrono>
#include <string>
#include <optional>

namespace ot
{
	class SSOAuthenticationProcessID
	{
		OT_DECL_NOCOPY(SSOAuthenticationProcessID)
		OT_DECL_DEFMOVE(SSOAuthenticationProcessID)
	public:
		SSOAuthenticationProcessID();
		SSOAuthenticationProcessID(const std::string& _token);

		std::string getID() const { return m_id; }

		bool isIsExpired() const;

		bool operator==(const SSOAuthenticationProcessID& other) const {
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
	struct hash<ot::SSOAuthenticationProcessID> {
		size_t operator()(const ot::SSOAuthenticationProcessID& k) const
		{
			return std::hash<std::string>{}(k.getID());
		}
	};
}
