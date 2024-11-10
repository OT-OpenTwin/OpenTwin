#pragma once

#include <map>
#include <list>

#include "RunIDContainer.h"

class InfoFileManager;

class Result1DManager
{
public:
	Result1DManager(char* dataBuffer, size_t dataBufferLength);
	Result1DManager() = delete;
	~Result1DManager();

	void addResult1DInformation(InfoFileManager& infoFileManager);
	std::list<int> getRunIDList();
	
	RunIDContainer* getContainer(int runID);

private:
	size_t readIntegerFromBuffer(size_t &bufferIndex, char* dataBuffer, size_t dataBufferLength);

	std::map<int, RunIDContainer*> runIDtoContainerMap;
};
