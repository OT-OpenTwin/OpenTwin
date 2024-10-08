#pragma once
#include <string>

class Varifier
{
public:
	void ensureCorrectMongoEnvVar();
	void setPermissions(const std::string& _path);

private:
	const std::string m_MongoAdressEnvVar = "OPEN_TWIN_MONGODB_ADDRESS";
};
