#pragma once

#include <string>
#include <map>
#include <vector>

#include "StudioSuiteConnector/Result1DFileData.h"

class Result1DRunIDContainer
{
public:
	Result1DRunIDContainer() : totalSize(0) {};
	~Result1DRunIDContainer();

	bool readData(const std::string& dataDir);
	void writeToBuffer(size_t runId, std::vector<char>& buffer);

	size_t getSize();
	size_t getNumberFiles();
	std::string getMetaDataHash();

private:
	void calculateMetaDataHash();
	void calculateSize(); 
	void writeIntegerToBuffer(size_t value, std::vector<char>& buffer);
	void writeStringToBuffer(std::string value, std::vector<char>& buffer);

	std::map<std::string, Result1DFileData*> fileNameToDataMap;
	std::string metaHashValue;
	size_t totalSize;
};

