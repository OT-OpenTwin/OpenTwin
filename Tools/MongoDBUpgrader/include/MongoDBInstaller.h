#pragma once
#include <string>
#include "MongoDBSettings.h"
class MongoDBInstaller
{
public:
	MongoDBInstaller(const std::string& _serverName);
	bool installNewVersion(const std::string& _installPath);
	void replaceMongoCfg(const std::string& _configContent);
private:
	const std::string& m_serverName;
};
