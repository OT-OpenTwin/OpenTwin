#pragma once

#include <map>

#include "RunIDContainer.h"

class Result1DManager
{
public:
	Result1DManager(char* dataBuffer, size_t dataBufferLength);
	Result1DManager() = delete;
	~Result1DManager();

private:
	size_t readIntegerFromBuffer(size_t &bufferIndex, char* dataBuffer, size_t dataBufferLength);

	std::map<size_t, RunIDContainer*> runIDtoContainerMap;
};
