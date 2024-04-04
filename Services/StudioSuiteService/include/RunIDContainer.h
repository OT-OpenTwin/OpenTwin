#pragma once

#include "Result1DData.h"

#include <string>
#include <map>

class RunIDContainer
{
public:
	RunIDContainer();
	~RunIDContainer();

	size_t readData(size_t& bufferIndex, char* dataBuffer, size_t dataBufferLength);

private:
	size_t readIntegerFromBuffer(size_t& bufferIndex, char* dataBuffer, size_t dataBufferLength);
	std::string readStringFromBuffer(size_t& bufferIndex, char* dataBuffer, size_t dataBufferLength);

	std::map<std::string, Result1DData*> fileNameToData;
};
