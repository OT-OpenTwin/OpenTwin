// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"

// std header
#include <optional>
#include <string>
#include <chrono>

namespace ot
{
	class SSOSessionToken
	{
		OT_DECL_NOCOPY(SSOSessionToken)
		OT_DECL_DEFMOVE(SSOSessionToken)
	public:
		SSOSessionToken() = default;
		~SSOSessionToken() = default;
		
		std::string generateToken();
		const std::string& getToken() const;

		bool tokenIsValid(bool _useDelta = true) const;
	private:
		std::string m_token;
		std::chrono::steady_clock::time_point m_creationTime;
		std::chrono::minutes m_tokenValidityDurationAbsolute{ 30 }; 
		std::chrono::seconds m_tokenValidityDelta{ 5 }; 
		// std::chrono::minutes m_tokenValidityDurationIdle{ 30 }; Does not make sense since the authentication requests happen rarely
	};
}