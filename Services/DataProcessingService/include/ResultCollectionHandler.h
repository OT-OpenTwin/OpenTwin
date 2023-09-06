#pragma once
#include <string>
#include <list>

class ResultCollectionHandler
{
public:
	ResultCollectionHandler();
	std::string getProjectCollection(const std::string& projectName);
	bool CollectionExists(const std::string& collectionName);
private:
	std::string _authorizationServiceURL;
};
