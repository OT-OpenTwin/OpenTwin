#include "LTSpiceConnector/Result1DFileManager.h"

#include <cassert>

Result1DFileManager::Result1DFileManager(const std::string& baseDir, std::list<int>& runIds)
{
	// In a first step, we need to recursively read the data for each runid
	for (auto id : runIds)
	{
		std::string runIdDir = baseDir + std::to_string(id);

		Result1DRunIDContainer* container = new Result1DRunIDContainer;
		bool success = container->readData(runIdDir);
		assert(success);

		assert(runIDToDataMap.count(id) == 0);
		runIDToDataMap[id] = container;
	}
}

Result1DFileManager::~Result1DFileManager()
{
	clear();
}

void Result1DFileManager::clear()
{
	for (auto item : runIDToDataMap)
	{
		if (item.second != nullptr)
		{
			delete item.second;
		}
	}

	runIDToDataMap.clear();
}

Result1DRunIDContainer *Result1DFileManager::getRunContainer(int id)
{
	if (runIDToDataMap.count(id) == 0) return nullptr;

	return runIDToDataMap[id];
}
