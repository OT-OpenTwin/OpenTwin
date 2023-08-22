#pragma once
#include <string>
#include <list>

class ResultCollectionHandler
{
public:
	std::list<std::string> getListOfProjects();
	std::string getProjectCollection(const std::string& projectName);

};
