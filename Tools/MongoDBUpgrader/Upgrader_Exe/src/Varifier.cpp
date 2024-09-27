#include "Varifier.h"
#include <stdlib.h>
#include <string>
#include "Logger.h"
#include "WindowsUtilityFuctions.h"
#include <iostream>



void Varifier::ensureCorrectMongoEnvVar()
{
	Logger::INSTANCE().write("Search for MongoDB address env var: " + m_MongoAdressEnvVar + "\n");
	
	
	char* envVar;
	size_t envVarSize;
	errno_t err = _dupenv_s(&envVar, &envVarSize, m_MongoAdressEnvVar.c_str());
	if (err != NULL)
	{
		throw std::exception(("Failed to get Mongo env var " + m_MongoAdressEnvVar).c_str());
	}
	if (envVar != nullptr)
	{
		std::string mongoPath(envVar);
		Logger::INSTANCE().write("Env var found:"+ mongoPath + "\n");
		if (mongoPath.find("tls@") != std::string::npos)
		{
			mongoPath = mongoPath.substr(4);
			Logger::INSTANCE().write("Changing env var to :" + mongoPath + "\n");
			wuf::RegistryHandler handler("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment");
			handler.setStringVal(m_MongoAdressEnvVar, mongoPath);
		}
	}
}
