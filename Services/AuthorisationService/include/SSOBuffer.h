#pragma once
#include <string>
#include <map>
#include <memory>
#include "OTSystem/SingleSignOn/SingleSignOn_Server.h"

class SSOBuffer
{
public:
	std::string handleRequest(const std::string& _username, const std::string& _receivedToken);
private:
	std::map<std::string, std::unique_ptr<ot::SingleSignOn_Server>> m_serverSignOnByUsername;
};
