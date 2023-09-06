#pragma once
#include <string>
#include <list>

class ResultCollectionHandler
{
public:
	ResultCollectionHandler();
	std::string getProjectCollection(const std::string& projectName);
private:
	std::string _authorizationServiceURL;
};
