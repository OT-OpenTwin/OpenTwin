#pragma once
#include <string>

class Varifier
{
public:
	void ensureCorrectMongoEnvVar();

private:
	const std::string m_MongoAdressEnvVar = "OPEN_TWIN_MONGODB_ADDRESS";
};
