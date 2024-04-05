#pragma once

#include "Result1DData.h"

#include <string>
#include <map>

class InfoFileManager;

class RunIDContainer
{
public:
	RunIDContainer();
	~RunIDContainer();

	int readData(size_t& bufferIndex, char* dataBuffer, size_t dataBufferLength);
	void addResult1DInformation(int runID, InfoFileManager& infoFileManager);

private:
	size_t readIntegerFromBuffer(size_t& bufferIndex, char* dataBuffer, size_t dataBufferLength);
	std::string readStringFromBuffer(size_t& bufferIndex, char* dataBuffer, size_t dataBufferLength);
	void processParameterInformation(const std::string& data);
	void clear();
	void readLine(std::stringstream& dataContent, std::string& line);

	std::string metaHashValue;
	std::map<std::string, Result1DData*> fileNameToData;
	std::map<std::string, double> parameterValues;
};
