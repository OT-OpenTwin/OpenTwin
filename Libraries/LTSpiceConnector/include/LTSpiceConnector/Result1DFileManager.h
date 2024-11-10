#pragma once

#include <string>
#include <list>
#include <map>

#include "LTSpiceConnector/Result1DRunIDContainer.h"


class Result1DFileManager
{
public:
	Result1DFileManager(const std::string& baseDir, std::list<int>& runIds);
	~Result1DFileManager();

	Result1DRunIDContainer* getRunContainer(int id);
	void clear();


private:
	std::map<int, Result1DRunIDContainer*> runIDToDataMap;
};

