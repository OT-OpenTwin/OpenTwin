// @otlicense

#pragma once
#include <string>
#include "MongoDBSettings.h"
class MongoDBInstaller
{
public:
	MongoDBInstaller(const std::string& _serverName);
	void replaceMongoCfg(const std::string& _configContent);
private:
	const std::string& m_serverName;
};
