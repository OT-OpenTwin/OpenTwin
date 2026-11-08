// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTCore/JSON/JSON.h"

// std header
#include <string>
#include <unordered_map>

class AccessTokenChallenge
{
	OT_DECL_NODEFAULT(AccessTokenChallenge)
	OT_DECL_NOCOPY(AccessTokenChallenge)
	OT_DECL_DEFMOVE(AccessTokenChallenge)
public:
	static AccessTokenChallenge createNewChallenge(const std::string& _id, const std::string& _token);

	bool operator==(const AccessTokenChallenge& _other) const { return m_id == _other.m_id; };

	bool isValid() const { return !m_id.empty() && !m_token.empty() && !m_challenge.empty(); };

	uint64_t getCreationTime() const { return m_creationTime; };
	const std::string& getID() const { return m_id; };
	const std::string& getToken() const { return m_token; };
	const std::string& getChallenge() const { return m_challenge; };

	// ###########################################################################################################################################################################################################################################################################################################################

private:
	uint64_t m_creationTime;
	std::string m_id;
	std::string m_token;
	std::string m_challenge;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Constructor

	AccessTokenChallenge(const std::string& _id, const std::string& _token, std::string&& _challenge);
};

namespace std
{
	template<>
	struct hash<AccessTokenChallenge>
	{
		std::size_t operator()(const AccessTokenChallenge& _challenge) const
		{
			return std::hash<std::string>()(_challenge.getID());
		}
	};
}