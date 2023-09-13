#pragma once
#include <string>
#include "Document/DocumentAccess.h"

class ResultCollectionAccess
{
public:
	ResultCollectionAccess();

private:
	std::string getProjectCollection(const std::string& projectName);
	bool CollectionExists(const std::string& collectionName);
	std::string _authorizationServiceURL;

};
