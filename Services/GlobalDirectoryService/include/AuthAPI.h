// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"

class AuthAPI
{
	OT_DECL_NOCOPY(AuthAPI)
	OT_DECL_NOMOVE(AuthAPI)
public:
	AuthAPI() = default;

	bool isValid() const { return !m_url.empty(); };

	void setUrl(const std::string& _url) { m_url = _url; };
	const std::string& getUrl() const { return m_url; };

	bool isGatewayTicketValid(const std::string& _user, const std::string& _ticket) const;

private:
	std::string m_url;

};

