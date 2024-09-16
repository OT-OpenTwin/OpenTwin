#include "MongoURL.h"
#include "Connection/ConnectionAPI.h"

std::string getMongoURL(std::string databaseURL, std::string dbUsername, std::string dbPassword)
{
	return DataStorageAPI::ConnectionAPI::getMongoURL(databaseURL, dbUsername, dbPassword);
}
